#ifndef STRINGREGISTRY_HPP_INCLUDED_
#define STRINGREGISTRY_HPP_INCLUDED_

// DUNE headers.
#include <DUNE/DUNE.hpp>

#include <string>
#include <map>
#include <set>
#include <vector>
#include <stdint.h>


/**
	Perfect string hasher library
**/

class StringRegistry
{

public:

	StringRegistry()
	{
		nextid = 1	;
		rt.push_back("");
	};
	/**Register new string and return new id **/
	std::size_t registerNew(std::string const& s)
	{
		std::size_t lookup = getId(s);

		if(lookup != 0)
			return lookup;

		stringidpair p;
		p.id = nextid++;
		p.s = s;
		std::size_t hashs = hash(s);//hasher(s);
		ft[hashs].insert(p);
		rt.push_back(s);
		return p.id;
	}

	std::size_t size() const
	{
		return nextid - 1;
	};
	std::size_t getId(std::string const& s) const
	{
		std::size_t hashs = hash(s);//hasher(s);
		fttype::const_iterator ftit = ft.find(hashs);

		if(ftit == ft.end())
		{
			return 0;
		}

		std::set<stringidpair>::const_iterator fit;

		for(fit = (*ftit).second.begin(); fit != (*ftit).second.end(); ++fit)
		{
			if((*fit).s == s)
				return (*fit).id;
		}

		return 0;
	}
	std::string getString(std::size_t id) const
	{
		if(id > rt.size())
			return "";

		return rt[id];
	}

private:
	typedef struct spair_s
	{
		std::string s;
		std::size_t id;
		bool operator<(struct spair_s const & a) const
		{
			return id < a.id;
		};
		bool operator==(struct spair_s const & a)const
		{
			return id == a.id;
		};
	} stringidpair;
	//From HASH to pair of ids,and strings for lookup
	typedef std::map<std::size_t, std::set<stringidpair> > fttype;
	//From id, to string
	typedef std::vector<std::string> rttype;
	//boost::hash<std::string> hasher;
	fttype ft;//from hash to id;
	rttype rt;//from id to string :)
	std::size_t nextid;
	std::size_t seed;

	static std::size_t hash(std::string const& s)
	{
		uint8_t md[16];//Result
		DUNE::Algorithms::MD5::compute((uint8_t*)s.c_str(), s.size(), md);
		std::size_t *rp=(std::size_t*)md;
		return *rp;

	}

};




#endif /* StringRegistry_H */
