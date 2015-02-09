#ifndef GRAMMAR_HPP_INCLUDED_
#define GRAMMAR_HPP_INCLUDED_
#include <ios>
#include <istream>
#include <iostream>
#include <limits>
#include <vector>
#include "StringRegistry.hpp"
#include "StringCast.hpp"

#define GRAMMAR_COMMENT_DELIM '%'
class Grammar
{

    public:
    struct Rule
    {
        std::size_t lhs;
        std::vector<std::size_t> rhs;
        float prob;
    };


    template<typename T>
    bool getToken(std::istream &is,T &read) const
    {
        //std::cout<<is.peek()<<std::endl;
        is >> read;
        is.clear();
        //std::cout<<is.peek()<<std::endl;

        if(is.peek()==GRAMMAR_COMMENT_DELIM)
        {
           is.clear();
           is.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
           return getToken<T>(is,read);
        }
        if(!is.good())
        {
            //std::cout<<"no good:"<<read<<std::endl;
            return false;
        }
        return true;

    }

    bool getToken(std::istream &is,std::string &read) const
    {
        is >> read;

        if(!is.good())
        {
            return false;
        }
        if(read.at(0)==GRAMMAR_COMMENT_DELIM)
        {

            is.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
           return getToken(is,read);
        }
        return true;

    }
    void skipComments(std::istream &is)
    {
        if(is.peek()==GRAMMAR_COMMENT_DELIM)
        {
            is.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
        }

    }
    void printRules()
    {
        for(unsigned i=0;i<allRules.size();i++)
        {
            std::cout<<symbols.getString(allRules[i].lhs)<< "->";
            for(unsigned j=0;j<allRules[i].rhs.size();j++)
                std::cout<< symbols.getString(allRules[i].rhs[j])<< " ";

            std::cout<<allRules[i].prob<<std::endl;
        }

    }


    bool load(std::istream &is)
    {
        std::string read;
        is>>std::skipws;
        //while(!is.eof())
        //{
            if(getToken(is,read)==false)
                return false;
            startsymbol=symbols.registerNew(read);
            //is>>read;
            //std::cout<<read<<"|"<<std::endl;
        //}
        unsigned numskip,temp;

        //std::cout<<"Reading numskip"<<numskip<<std::endl;
        if(getToken<unsigned>(is,numskip)==false)
                return false;
        //std::cout<<"Skipping"<<numskip<<std::endl;
        for(unsigned i=0;i<2*numskip;i++)
        {
            if(getToken<unsigned>(is,temp)==false)
                return false;
        }

        while(!is.eof())
        {
            //std::cout<<"Reading New Set"<<"|"<<is.peek()<<"|"<<std::endl;
            if(getToken(is,read)==false)
                return false;
            std::size_t lhs=symbols.registerNew(read);
            //std::cout<<"lhs: "<<lhs<<std::endl;
            unsigned numOfRules;
            if(getToken<unsigned>(is,numOfRules)==false)
                return false;
            //std::cout<<"numOfRules: "<<numOfRules<<std::endl;


            for(unsigned i=0;i<numOfRules;i++)
            {
                unsigned numofrhs;
                if(getToken<unsigned>(is,numofrhs)==false)
                    return false;
                Rule newrule;
                newrule.lhs=lhs;
                newrule.rhs.reserve(numofrhs);
                for(unsigned j=0;j<numofrhs;j++)
                {
                     if(getToken(is,read)==false)
                        return false;
                    std::size_t rhs_j=symbols.registerNew(read);
                    newrule.rhs.push_back(rhs_j);
                }
                float prob;
                if(getToken<float>(is,prob)==false)
                        return false;
                newrule.prob=prob;
                //Skip T/NT
                if(getToken(is,read)==false)
                        return false;
                allRules.push_back(newrule);
                is.ignore( std::numeric_limits<std::streamsize>::max(),'\n');
                skipComments(is);
            }


        }
        return true;
    }

    void convertToChomsky()
    {
        std::vector<Rule> oldRules=allRules;
        allRules.clear();
        for(unsigned i=0;i<oldRules.size();i++)
        {

            if(oldRules[i].rhs.size()>2)
            {
                Rule r= oldRules[i];
                std::size_t lhs=r.lhs;
                for(unsigned j=0;j<r.rhs.size()-2;j++)
                {
                    Rule newRule;
                    newRule.lhs=lhs;
                    std::string name=symbols.getString(r.lhs);
                    name.append("_");
                    name.append(toString(i));
                    name.append("_");
                    name.append(toString(j));
                    std::size_t newsymbol=symbols.registerNew(name);
                    newRule.rhs.reserve(2);
                    newRule.rhs.push_back(r.rhs[j]);
                    newRule.rhs.push_back(newsymbol);
                    newRule.prob=1;
                    lhs=newsymbol;
                    allRules.push_back(newRule);
                }

                Rule newRule;
                newRule.lhs=lhs;
                newRule.rhs.reserve(2);
                newRule.rhs.push_back(r.rhs[r.rhs.size()-2]);
                newRule.rhs.push_back(r.rhs[r.rhs.size()-1]);
                newRule.prob=r.prob;
                allRules.push_back(newRule);

            }
            else
                allRules.push_back(oldRules[i]);

        }

    }


    StringRegistry symbols;
    std::size_t startsymbol;
    std::vector<Rule> allRules;





};
#endif
