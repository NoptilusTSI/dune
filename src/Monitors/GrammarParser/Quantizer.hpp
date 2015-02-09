#ifndef QUANTIZER_HPP_INCLUDED_
#define QUANTIZER_HPP_INCLUDED_
#include <fstream>
class Quantizer
{
	std::vector<float> thresh;
	std::vector<std::string> symbols;
	std::size_t ttl;


    public:

	bool load(std::istream &is)
    {
        std::string read;
        //int rint;
        //float rfloat;
        is>>ttl;
        thresh.resize(ttl+1);
        symbols.resize(ttl);
        for(unsigned i=0;i<ttl;i++)
        {
			//std::cout<<"QUANTIZER LOADED:"<<thresh[i]<<" "<<symbols[i]<<" "<<is.good()<<std::endl;
			is>>thresh[i];
			is>>symbols[i];
			//std::cout<<"QUANTIZER LOADED:"<<thresh[i]<<" "<<symbols[i]<<" "<<is.good()<<std::endl;
		}
		is>>thresh[ttl];
		return true;

    }
    std::string quantize(float v)
    {
		//std::cout<<"TOTAL TTL:"<<ttl<<std::endl;
        for(unsigned i=1;i<ttl+1;i++)
        {
			//std::cout<<"Threshold i:"<<thresh[i]<<std::endl;
            if(thresh[i]>=v)
                return symbols[i-1];
        }
        return "";

	}


};


#endif
