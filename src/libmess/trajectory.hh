/*
        Chemical Kinetics and Dynamics Library
        Copyright (C) 2008-2013, Yuri Georgievski <ygeorgi@anl.gov>

        This library is free software; you can redistribute it and/or
        modify it under the terms of the GNU Library General Public
        License as published by the Free Software Foundation; either
        version 2 of the License, or (at your option) any later version.

        This library is distributed in the hope that it will be useful,
        but WITHOUT ANY WARRANTY; without even the implied warranty of
        MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
        Library General Public License for more details.
*/

#ifndef TRAJECTORY_HH
#define TRAJECTORY_HH

#include "dynamic.hh"
#include "slatec.hh"
#include "potential.hh"

#include <setjmp.h>

enum {BACKWARD = 0, FORWARD = 1};

namespace Trajectory {

  class Exception {};
  class PotentialFailure : public Exception {};
  class ExcludeRegionHit : public Exception {};
  class       RunFailure : public Exception {};

  // dynamic variables derivatives calculator
  extern "C" void set_dvd (const double& time, const double* dv, double* dvd, void*, void* par);

// container to transfer data between dvd calculator and the calling function
struct DvdPar {
    Potential::Wrap pot;
    double ener; // energy
    jmp_buf jmp; // jump buffer for for long jumps
};


  // Output Flags and streams
  struct Flags {
    std::pair<int, SharedPointer<std::ostream> > aux_out;
    // ...
  };

  class Propagator : public Dynamic::Vars, public Slatec::AdamSolver
  {
    int    _dir;
    int    _spec;
    double _time;
    double _ener;
    
    Potential::Wrap _pot;

  public:

    static double step;
    //static Flags flags;

    Propagator(Potential::Wrap pot, const Dynamic::Vars& dv, const Slatec::AdamSolver& as, int dir) 
      : _pot(pot), Dynamic::Vars(dv), Slatec::AdamSolver(as), _time(0.0), _dir(dir) {}

    Propagator(Potential::Wrap pot, const Dynamic::Vars& dv, int dir) 
      : _pot(pot), Dynamic::Vars(dv), Slatec::AdamSolver(Structure::dv_size(), set_dvd), 
	_time(0.0), _dir(dir) {}

    Propagator(Potential::Wrap pot, const Dynamic::Vars& dv, int dir, double rt, double at) 
      : _pot(pot), Dynamic::Vars(dv), Slatec::AdamSolver(Structure::dv_size(), set_dvd, rt, at), 
	_time(0.0), _dir(dir) {}

    void run (Dynamic::CCP stop, const Dynamic::Classifier& sort) ;

    double time         () const { return _time; }
    int    direction    () const { return _dir; }

    double total_energy () const { return _ener; }
    int    species      () const { return _spec; }
  };

}

#endif
