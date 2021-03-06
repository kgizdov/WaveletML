#ifndef WAVELETML_WaveletML
#define WAVELETML_WaveletML

/**
 * @file WaveletML.h
 * @author Andreas Sogaard
**/

// STL include(s).
#include <fstream>
#include <iostream>
#include <stdio.h> /* printf */
#include <vector>
#include <cmath> /* log2 */
#include <assert.h>     /* assert */
#include <stdlib.h> /* system */

// ROOT include(s).
#include "TH2.h"
#include "TGraph.h"

// Armadillo include(s).
#include <armadillo>

// WaveletML include(s).
#include "Utils.h"
#include "MatrixOperator.h"
#include "LowpassOperator.h"
#include "HighpassOperator.h"
#include "Snapshot.h"

class Snapshot;

using namespace std;
using namespace arma;

class WaveletML {
    
    friend class Snapshot;
    
public:
    
    // Constructor(s).
    WaveletML () {};
    WaveletML (const double& lambda) :
        _lambda(lambda)
    {};
    WaveletML (const double& lambda, const double& alpha) :
        _lambda(lambda), _alpha(alpha)
    {};
    WaveletML (const double& lambda, const double& alpha, const double& inertia) :
        _lambda(lambda), _alpha(alpha), _inertia(inertia)
    {};
    WaveletML (const WaveletML& other) :
        _lambda(other._lambda), _alpha(other._alpha), _inertia(other._inertia), _filter(other._filter)
    {};
    
    // Destructor.
    ~WaveletML () {};
    
    // Get method(s).
    inline double getLambda () { return _lambda; }
    inline double    lambda () { return getLambda(); }

    inline double getAlpha () { return _alpha; }
    inline double    alpha () { return getAlpha(); }

    inline double getInertia () { return _inertia; }
    inline double    inertia () { return getInertia(); }
    
    inline arma::Col<double> getFilter () { return _filter; }
    inline arma::Col<double>    filter () { return getFilter(); }
    
    inline arma::Col<double> getMomentum () { return _momentum; }
    inline arma::Col<double>    momentum () { return getMomentum(); }

    inline int getBatchSize () { return _batchSize; }
    inline int    batchSize () { return getBatchSize(); }

    inline std::vector< arma::Col<double> > getFilterLog () { return _filterLog; }
    inline std::vector< arma::Col<double> >    filterLog () { return getFilterLog(); }
    inline void                           clearFilterLog () { return _filterLog.clear(); }

    inline std::vector< double > getCostLog () { return _costLog; }
    inline std::vector< double >    costLog () { return getCostLog(); }
    inline void                clearCostLog () { return _costLog.resize(1, 0); }

    
    // Set method(s).
    bool setLambda    (const double& lambda);
    bool setAlpha     (const double& alpha);
    bool setInertia   (const double& inertia);
    bool setFilter    (const arma::Col<double>& filter);
    bool setMomentum  (const arma::Col<double>& momentum);
    bool setBatchSize (const unsigned& batchSize);
    bool doWavelet    (const bool& wavelet);
    
    // Print method(s).
    void print ();

    // Storage method(s).
    inline void saveAs (const string& filename) { save(filename); return; };
    void        save   (const string& filename);
    void        load   (const string& filename);
    
    // High-level learning methods(s).
    // -- 1D.
    arma::field< arma::Col<double> > forward (const arma::Col<double>& x);

    arma::Col<double>                inverse (const arma::field< arma::Col<double> >& activations);
    arma::Col<double>                inverse (const arma::Col<double>& y);

    // -- 2D.
    /*arma::Mat<double>                               forward (const arma::Mat<double>& X);*/
    arma::field< arma::field< arma::Col<double> > > forward (const arma::Mat<double>& X);
    arma::Mat<double>                               inverse (const arma::Mat<double>& Y);

    
    // High-level cost method(s).
    double GiniCoeff (const arma::Col<double>& y);
    double GiniCoeff (const arma::Mat<double>& Y);

    arma::Col<double> GiniCoeffDeriv (const arma::Col<double>& y);
    arma::Mat<double> GiniCoeffDeriv (const arma::Mat<double>& Y);

    double SparseTerm (const arma::Col<double>& y);
    double SparseTerm (const arma::Mat<double>& Y);
    double RegTerm    (const arma::Col<double>& y);

    arma::Col<double> SparseTermDeriv (const arma::Col<double>& y);
    arma::Mat<double> SparseTermDeriv (const arma::Mat<double>& Y);
    arma::Col<double> RegTermDeriv    (const arma::Col<double>& y);

    double cost (const arma::Col<double>& y);
    double cost (const arma::Mat<double>& Y);
    
    arma::field< arma::Mat<double> > costMap (const arma::Mat<double>& X, const double& range, const unsigned& Ndiv);
    arma::field< arma::Mat<double> > costMap (const std::vector< arma::Mat<double> >& X, const double& range, const unsigned& Ndiv);
    
    // High-level basis method(s).
    arma::Col<double> basisFct (const unsigned& N, const unsigned& i);
    arma::Mat<double> basisFct (const unsigned& N, const unsigned& i, const unsigned& j);
    
    TGraph getCostGraph (const std::vector< double >& costLog);
    TGraph getCostGraph (const std::vector< arma::Col<double> >& filterLog, const std::vector< arma::Mat<double> >& X);
    
public: // PROTECTED
    
    // Low-level learning method(s).
    void addMomentum   (const arma::Col<double>& momentum);
    void scaleMomentum (const double& factor);
    void clear ();
    void update (const arma::Col<double>& gradient);
    
    void cacheOperators (const unsigned& m);
    void clearCachedOperators ();
    
    void cacheWeights (const unsigned& m);
    void clearCachedWeights ();
    
    arma::Col<double> lowpassfilter      (const arma::Col<double>& x);
    arma::Col<double> highpassfilter     (const arma::Col<double>& x);
    arma::Col<double> inv_lowpassfilter  (const arma::Col<double>& y);
    arma::Col<double> inv_highpassfilter (const arma::Col<double>& y);

    const arma::Mat<double>& lowpassweight      (const unsigned& level, const unsigned& filt);
    const arma::Mat<double>& highpassweight     (const unsigned& level, const unsigned& filt);

    
    arma::field< arma::Col<double> >  ComputeDelta (const arma::Col<double>& delta, arma::field< arma::Col<double> > activations);
    
    void batchTrain (arma::Mat<double> X);
    void flushBatchQueue ();
    
    
    // Miscellaneous.
    arma::Col<double> coeffsFromActivations (const arma::field< arma::Col<double> >& activations);
    arma::Mat<double> coeffsFromActivations (const arma::field< arma::field< arma::Col<double> > >& Activations);
    
private: // PRIVATE
    
    double _lambda  = 10.0;
    double _alpha   =  0.05;
    double _inertia =  0.;
    arma::Col<double> _filter;
    arma::Col<double> _momentum;

    bool _hasCachedOperators = false;
    arma::field< arma::Mat<double> > _cachedLowpassOperators;
    arma::field< arma::Mat<double> > _cachedHighpassOperators;

    bool _hasCachedWeights = false;
    arma::field< arma::Mat<double> > _cachedLowpassWeights;
    arma::field< arma::Mat<double> > _cachedHighpassWeights;

    unsigned _batchSize = 1;
    std::vector< arma::Col<double> > _batchQueue;
    std::vector< arma::Col<double> > _filterLog;
    std::vector< double >            _costLog;
    
    bool _wavelet = false;
    
};

#endif
