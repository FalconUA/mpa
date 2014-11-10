#include "defines.hpp"
#include "mpr.hpp"
#include <iostream>

namespace mp{

mpr cooknewton(const mpr& V, const mpr& z, size_t depth){
    if(depth >= 50){
        return z;
	}
    mpr X;
	X = z*2 - V*z*z;

    if(X.length() > 100){
        for(size_t j = 0, i = X.length() - 99; j < 100; j++&&i++)
            X.value[j] = X.value[i];
        X.value.resize(100);
        X.Exp = 99;
    }

    depth++;
    X = cooknewton(V,X,depth);
    return X;
}

mpr inverse(const mpr& num){
    mpr res, V(num);
    V.Sign = mp::PLUS;

    mpr z;
	z = 1;

	size_t deg = V.Exp;
	V << deg;
	V.Exp = V.length();

    res = cooknewton(V,z,1);
    res.Exp = res.Exp + V.length();
    res >> 1;
    res << deg + 1;
    res.Sign = num.Sign;
	res.SplitDepth = num.SplitDepth;	
    return res;
}


}; // end of namespace mp
