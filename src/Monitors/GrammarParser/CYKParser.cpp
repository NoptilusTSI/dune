#include "CYKParser.hpp"


void CYKParser::fixClosure(ChartList &u)
{
    std::vector<bool>skiplist(unitaryRules.size(),false);//Fill constructor
    for(unsigned i=0;i<unitaryRules.size();)
    {
        if(skiplist[i]==true)
        {
            i++;
            continue;
        }
        Grammar::Rule &r=unitaryRules[i];
        bool changes=false;
        for(unsigned j=0;j<u.size();j++ )
        {
            if(u[j].symbol==r.rhs[0])
            {
                ChartEntry n;
                n.prob=r.prob*u[j].prob;
                n.symbol=r.lhs;
                n.rule_no=0;
                u.push_back(n);
                changes=true;
            }
        }
        if(changes)
        {
            skiplist[i]=true;
            i=0;
        }
        else
            i++;

    }
    //Remove duplicates created due to multiple transtive paths

   u=trimDuplicates(u);
}
CYKParser::ChartList CYKParser::trimDuplicates(CYKParser::ChartList &u)
{

    ChartList c;
    for(unsigned i=0;i<u.size();i++)
    {
        bool addnew=true;
        for(unsigned j=0;j<c.size();j++)
        {
            if(c[j].symbol==u[i].symbol)
            {
                c[j].prob+=u[i].prob;
                addnew=false;
                break;
            }

        }
        if(addnew)
            c.push_back(u[i]);
    }
    return c;

}
void CYKParser::printUnitary()
{

    for(unsigned i=1;i<symbolcount;i++)
    {
        std::cout<<g.symbols.getString(i)<< "<-" <<"("<<unitaryClosure[i].size()<<") ";
        for(unsigned j=0;j<unitaryClosure[i].size();j++)
            std::cout<<g.symbols.getString(unitaryClosure[i][j].symbol)<< "("<<unitaryClosure[i][j].prob<<") ";
        std::cout<<std::endl;
    }


}

void CYKParser::printBinary()
{

    for(unsigned i=1;i<symbolcount;i++)
    {

        for(unsigned j=1;j<symbolcount;j++)
        {
            std::cout<<g.symbols.getString(i)<<","<<g.symbols.getString(j)<<"<-";
            std::vector< std::size_t > &l=binaryRuleMap[i][j];
            for(unsigned k=0;k<l.size();k++)
                std::cout<<g.symbols.getString(g.allRules[l[k]].lhs)<<" ";
            std::cout<<std::endl;
        }
    }


}


void CYKParser::printRoot()
{
    if(loadedSymbols==0)
        return;
    ChartList &l=parseChart[0][loadedSymbols-1];
    for(unsigned i=0;i<l.size();i++)
    {
        std::cout<<g.symbols.getString(l[i].symbol)<<" ("<<l[i].prob<<")"<<std::endl;

    }

}


float CYKParser::rootProbability()
{
    if(loadedSymbols==0)
        return 0;
    ChartList &l=parseChart[0][loadedSymbols-1];
    for(unsigned i=0;i<l.size();i++)
    {
        if(l[i].symbol==g.startsymbol)
            return l[i].prob;
        //std::cout<<g.symbols.getString(l[i].symbol)<<" ("<<l[i].prob<<")"<<std::endl;

    }
    return 0;

}
std::string CYKParser::mostLikelySymbol()
{
    if(loadedSymbols==0)
        return "";
      ChartList &l=parseChart[0][loadedSymbols-1];
    std::size_t best=0;
    float bestProb=0;
    for(unsigned i=0;i<l.size();i++)
    {
        for(unsigned j=0;j<unitaryRules.size();j++)
        {
            Grammar::Rule &r=unitaryRules[j];
            if(r.lhs==g.startsymbol&&r.rhs[0]==l[i].symbol)
            {
                float newprob=l[i].prob*r.prob;
                if(newprob>bestProb)
                {
                    best=l[i].symbol;
                    bestProb=newprob;
                }


            }

        }

    }
    if(best==0)
        return "";
    //std::size_t ruleNo=l[best].rule_no;
    return g.symbols.getString(best);




}
void CYKParser::parseSymbol(std::string symbol)
{
    std::size_t newsymbol= g.symbols.getId(symbol);
    //std::cout<<"Symbol "<< "|"<<symbol<<"|"<<newsymbol<<" "<<loadedSymbols<<std::endl;
    assert(newsymbol>0);
    if(w==loadedSymbols) //Drop oldest if
    {
        for(unsigned i=0;i<w-1;i++)
        {

            parseChart[i]=parseChart[i+1];

            for(unsigned j=0;j<w-1;j++)
                parseChart[i][j]=parseChart[i][j+1];
            parseChart[i][w-1].clear(); //Clear last column
        }
        for(unsigned j=0;j<w;j++)
           parseChart[w-1][j].clear(); //Clear last row

        loadedSymbols--;
        //std::cout<<"Clean Done"<<std::endl;
    }

    unsigned s=loadedSymbols;
    ChartEntry n;
    n.symbol=newsymbol;
    n.prob=1;
    n.rule_no=g.allRules.size();
    parseChart[s][s].clear();
    parseChart[s][s].push_back(n);

    //Expand closure and append it
    parseChart[s][s].insert(parseChart[s][s].end(),
                            unitaryClosure[newsymbol].begin(),
                            unitaryClosure[newsymbol].end());
    parseChart[s][s]=trimDuplicates(parseChart[s][s]);

    for(int i=s-1;i>=0;i--)
    {
        ChartList newlist;
        for(unsigned j=i;j<s;j++)
        {
            for(unsigned k=0;k<parseChart[i][j].size();k++)
            {
                for(unsigned l=0;l<parseChart[j+1][s].size();l++)
                {
                    ChartEntry &le=parseChart[i][j][k];
                    ChartEntry &re=parseChart[j+1][s][l];

                    //std::cout<<"Symbols"<<le.symbol<<" "<<re.symbol<<std::endl;
                    std::vector<std::size_t> &rulelst=binaryRuleMap[ le.symbol] [re.symbol];
                    //std::cout<<"size:"<<rulelst.size()<<std::endl;
                    for(unsigned m=0;m<rulelst.size();m++)
                    {
                        ChartEntry newentry;
                        //std::cout<<rulelst[m]<<std::endl;
                        assert(rulelst[m]<g.allRules.size());
                        newentry.symbol=g.allRules[rulelst[m]].lhs;
                        newentry.rule_no=rulelst[m];
                        newentry.prob=g.allRules[rulelst[m]].prob*le.prob*re.prob;
                        newlist.push_back(newentry);
                    }
                }


            }
        }

        //Now expand list using unitary Closure
        std::size_t ss=newlist.size();
        newlist=trimDuplicates(newlist);
        //std::cout<<"newlist size:"<<ss<<std::endl;
        for(unsigned o=0;o<ss;o++)
        {
            float prior=newlist[o].prob;
            ChartList &closure=unitaryClosure[newlist[o].symbol];
            for(unsigned p=0;p<closure.size();p++)
            {
                ChartEntry d=closure[p];
                d.prob=d.prob*prior;
                newlist.push_back(d);
            }
            /*newlist.insert(newlist.end(),
                            unitaryClosure[newlist[o].symbol].begin(),
                            unitaryClosure[newlist[o].symbol].end()
                            );
            */
        }

        parseChart[i][s]=trimDuplicates(newlist);
        //parseChart[i][s]=newlist;


    }

    loadedSymbols++;

}
