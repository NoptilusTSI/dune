#include "StringRegistry.hpp"
#include "Grammar.hpp"

class CYKParser
{
    public:
    class ChartEntry
    {
        public:
        std::size_t rule_no;
        std::size_t symbol;
        float prob;

        ChartEntry() :rule_no(0),symbol(0),prob(0)
        {};
    };
    typedef std::vector<ChartEntry> ChartList;

    class ParseEntry
    {

    };
    void resize(unsigned s)
    {
        w=s;
        parseChart.clear();
        parseChart.resize(w);
        for(unsigned i=0;i<w;i++)
        {
            parseChart[i].resize(w);
            for(unsigned j=0;j<w;j++)
                parseChart[i][j].clear();
        }
        loadedSymbols=0;
    }

    void reset() { resize(w);};

    void loadGrammar(Grammar const&ag)
    {
        g=ag;
        unitaryRules.clear();
        symbolcount=g.symbols.size()+1;
        //std::cout << "-----------------------------"<<g.symbols.getString(symbolcount-1)<<std::endl;
        unitaryClosure.resize(symbolcount);
        for(unsigned i=0;i<g.allRules.size();i++)
        {
            const Grammar::Rule &r=g.allRules[i];
            if(r.rhs.size()==1)
            {
                unitaryRules.push_back(r);
                ChartEntry e;
                e.symbol=r.lhs;
                e.rule_no=0;
                e.prob=r.prob;
                unitaryClosure[r.rhs[0]].push_back(e);

            }

        }
        //std::cout<<"CHECK1"<<std::endl;
        unitaryTransitiveClosure();
        //std::cout<<"CHECK2"<<std::endl;
        //Binary Rules now...
        binaryRuleMap.resize(symbolcount);
        for(unsigned i=0;i<symbolcount;i++)
        {
            binaryRuleMap[i].resize(symbolcount);
            for(unsigned j=0;j<symbolcount;j++)
                binaryRuleMap[i][j].clear();
        }
        for(unsigned i=0;i<g.allRules.size();i++)
        {
            Grammar::Rule &r=g.allRules[i];
            if(r.rhs.size()!=2)
                continue;
            //std::cout<<g.symbols.getString(symbolcount)<<std::endl;
            assert(r.rhs[0]<symbolcount);
            assert(r.rhs[1]<symbolcount);
            binaryRuleMap[r.rhs[0]][r.rhs[1]].push_back(i);//Rule number

        }

    }

    void parseSymbol(std::string symbol);

    void unitaryTransitiveClosure()
    {
        for(unsigned i=1;i<symbolcount;i++)
        {
            fixClosure(unitaryClosure[i]);
        }

    }
    void fixClosure(ChartList &u);
    void printUnitary();

    void printBinary();
    void printRoot();

    float rootProbability();
    std::string mostLikelySymbol();

    static ChartList trimDuplicates(ChartList &u);

    std::vector< std::vector<ChartList> > parseChart;
    unsigned w; //Window size
    unsigned loadedSymbols;
    unsigned symbolcount;
    Grammar g;




    std::vector<ChartList> unitaryClosure;
    std::vector<Grammar::Rule> unitaryRules;
    std::vector< std::vector< std::vector<std::size_t> > > binaryRuleMap;



};
