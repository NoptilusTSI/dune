
// ISO C++ 98 headers.
#include <cmath>
#include "StringRegistry.hpp"
#include "CYKParser.hpp"
#include "Grammar.hpp"
#include "IPreprocessor.hpp"
#include "Preprocessors.hpp"
// DUNE headers.
#include <DUNE/DUNE.hpp>

//Stl string comparisson
#include <algorithm>
using namespace std;


template<class TContainer>
bool begins_with(const TContainer& input, const TContainer& match)
{
    return input.size() >= match.size()
        && equal(match.begin(), match.end(), input.begin());
}


namespace Monitors
{
	//! PCFG Parser for Dune
  namespace GrammarParser
  {
    using DUNE_NAMESPACES;


    struct Arguments
    {
      //! What Message to parse
      std::vector<std::string> bind_messages;

     std::string  grammar_filename;
     std::vector<std::string>  quant_filename;
      std::string log_file;

      bool normal_abnormal_mode;
      float normal_threshold;
      unsigned window_size;
      bool rolling_window;
    };

    struct Task: public  DUNE::Tasks::Periodic
    {
      //! Task arguments.
      Arguments m_args;
      Grammar grammar;
      CYKParser parser;
      ConcatPreprocessor *preprocess;
      std::ofstream logofs;
      bool initOK;


      Task(const std::string& name, Tasks::Context& ctx):
        DUNE::Tasks::Periodic(name, ctx)
      {
        initOK=false;
        preprocess=NULL;
        param("Bind to", m_args.bind_messages)
        .defaultValue("")
        .description("List of messages to bind");

        /*param("Entity Label", m_args.entity_label)
        .defaultValue("")
        .description("Entity name of output message");*/

        param("Grammar", m_args.grammar_filename)
        .defaultValue("")
        .description("Grammar file to load");

        param("Quantization", m_args.quant_filename)
        .defaultValue("")
        .description("Quantization file to load");

        param("Log", m_args.log_file)
        .defaultValue("")
        .description("Quantization file to load");

        param("Normal Abnormal", m_args.normal_abnormal_mode)
        .defaultValue("true")
        .description("Grammar mode set to normal abnormal");


        param("Normal Threshold", m_args.normal_threshold)
        .defaultValue("")
        .description("Grammar mode set to normal abnormal");

        param("Window Size", m_args.window_size)
        .defaultValue("0")
        .description("Grammar parse window");

        param("Rolling Window", m_args.rolling_window)
        .defaultValue("true")
        .description("Grammar rolling window");


      }

      void
      onUpdateParameters(void)
      {

        if(m_args.bind_messages.size()==0)
        {
            setEntityState(IMC::EntityState::ESTA_FAULT, Status::CODE_IO_ERROR);
            return ;
        }

        if(m_args.bind_messages.size()==1)
        {


        }
        if(m_args.bind_messages.size()>0)
        {
            preprocess= new ConcatPreprocessor();
            for(unsigned i=0;i<m_args.bind_messages.size();i++)
            {

                if(m_args.bind_messages[i]=="Depth")
                {
                    preprocess->append(new DepthPreprocessor());
                }

                if(m_args.bind_messages[i]=="Pitch")
                {
                    preprocess->append(new PitchPreprocessor());
                }
                std::string hierarchicalname="ParserOutput.";
                if(begins_with(m_args.bind_messages[i],hierarchicalname))
                {
                    HierarchicalPreprocessor *p = new HierarchicalPreprocessor();
                    p->setEntityLabel(m_args.bind_messages[i].substr(hierarchicalname.size()));
                    preprocess->append(p);
                }
            }

        }


        if(preprocess)
        {
             std::vector<std::string> b=preprocess->getBindMessages();
            bind(this, b);
            //std::cout<<"Binding to:"<<b[0]<<std::endl;
        }
        //bind(this, m_args.bind_messages);

        //std::cout<<"ONUPDATEPARAMETERS"<<std::endl;
      }

      void
      onEntityResolution(void)
      {

      }

      void
      onEntityReservation(void)
      {

      }

      void
      onResourceRelease(void)
      {

      }

      void
      onResourceAcquisition(void)
      {

      }

      void
      onResourceInitialization(void)
      {


        FileSystem::Path grammarpath=m_ctx.dir_cfg/ m_args.grammar_filename;

        FileSystem::Path logpath=m_ctx.dir_log/ m_args.log_file;
        if(!grammarpath.isFile())
        {
			war("Cannot Load Grammar File: %s",m_args.grammar_filename.c_str());
            setEntityState(IMC::EntityState::ESTA_FAULT, Status::CODE_IO_ERROR);
            initOK=false;
            return ;
        }

        std::ifstream gifs(grammarpath.c_str());
        if(grammar.load(gifs)==false)
        {
            war("Cannot Parse Grammar File: %s",m_args.grammar_filename.c_str());
             setEntityState(IMC::EntityState::ESTA_FAULT, Status::CODE_IO_ERROR);
            gifs.close();
            initOK=false;
            return ;
        }

        if(preprocess)
        {

            for(unsigned i=0;i<m_args.quant_filename.size();i++)
            {
                if(m_args.quant_filename[i]=="") //No quantizer defined for this, dont care
                    continue;
                FileSystem::Path quant=m_ctx.dir_cfg/ m_args.quant_filename[i];

                if(!quant.isFile())
                {
					war("Cannot Load Quantization File: %s",m_args.quant_filename[i].c_str());
                    setEntityState(IMC::EntityState::ESTA_FAULT, Status::CODE_IO_ERROR);
                    initOK=false;
                    return ;
                }
                std::ifstream quantifs(quant.c_str());
                if(preprocess->concat[i]->load(quantifs)==false)
                  {
					war("Cannot Parse Quantization File: %s",m_args.quant_filename[i].c_str());
                    setEntityState(IMC::EntityState::ESTA_FAULT, Status::CODE_IO_ERROR);
                    quantifs.close();
                    initOK=false;
                    return ;
                }
                quantifs.close();
            }


        }

        logofs.open(logpath.c_str());
        logofs<<std::setprecision(20);



        grammar.convertToChomsky();


        parser.loadGrammar(grammar);
        parser.resize(m_args.window_size);


        inf("Loaded Configuration Files");
        initOK=true;
        setEntityState(IMC::EntityState::ESTA_BOOT, Status::CODE_INIT);

      }

      void
      consume(const IMC::Message* msg)
      {

          if(preprocess&&initOK)
          {
            //inf("Received  msg: %s",IMC::Factory::getAbbrevFromId(msg->getId()).c_str());
            preprocess->prerocess(msg);
          }
      }


      void
      task(void)
      {

        if(preprocess&&initOK&&preprocess->symbolReady())
        {

            std::string s=preprocess->getSymbol();
            //std::cout<<"Run:"<<s<<std::endl;
            parser.parseSymbol(s);
            //parser.printRoot();
            if(m_args.normal_abnormal_mode)
            {

                if(parser.rootProbability()>m_args.normal_threshold)
                    dispatchOutput("N");
                else
                    dispatchOutput("A");

            }
            else
            {
                std::string symbol=parser.mostLikelySymbol();
                if(symbol.size()>0)
                {
                    dispatchOutput(symbol);
                }

            }
        }

      }

        void dispatchOutput(std::string s)
        {
            IMC::ParserOutput out;
            out.result=s;
            out.source_entity=getEntityLabel();
            dispatch(out);
            if(logofs.is_open())
            {
                if(preprocess)
                {
                    //std::cout<<preprocess->getTimeStamp()<<std::endl;
					logofs<<preprocess->getTimeStamp()<<",";
                    for(unsigned i=0;i<preprocess->concat.size();i++)
                        logofs<<preprocess->concat[i]->getRaw()<<",";
                    for(unsigned i=0;i<preprocess->concat.size();i++)
                        logofs<<preprocess->concat[i]->getSymbol()<<",";
                    logofs<<grammar.symbols.getId(s)<<std::endl;

                }

            }

        }
    };
  }
}

DUNE_TASK
