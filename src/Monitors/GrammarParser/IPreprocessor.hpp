#ifndef IPREPROCESSOR_HPP_INCLUDED_
#define IPREPROCESSOR_HPP_INCLUDED_

#include <DUNE/DUNE.hpp>


class IPreprocessor
{
    public:
    virtual ~IPreprocessor() {};
	double last_timestamp;
	virtual double getTimeStamp() { return last_timestamp; };
    virtual std::string getSymbol() =0;
    virtual bool symbolReady() =0;
    virtual std::vector<std::string> getBindMessages()=0;

    virtual bool preprocess(const DUNE::IMC::Message* msg) =0;

    virtual bool load(std::ifstream &ifs)=0;
    virtual std::string getRaw() =0;


};
#endif
