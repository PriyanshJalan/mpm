#ifndef MPM_MPM_H_
#define MPM_MPM_H_

#include <chrono>
#include <memory>
#include <vector>

#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "io.h"
#include "mesh.h"
#include "read_mesh.h"
#include "read_mesh_ascii.h"

#ifdef USE_VTK
#include "vtk_writer.h"
#endif

namespace mpm {
//! MPM class
//! \brief MPM class calls solver and algorithm
//! \details MPM class: implicit and explicit MPM
class MPM {
 public:
  //! Constructor
  MPM(const std::shared_ptr<IO>& io) : io_(io) {

    analysis_ = io_->analysis();

    // Unique id
    if (analysis_.find("uuid") != analysis_.end())
      uuid_ = analysis_["uuid"].template get<std::string>();

    if (uuid_.empty())
      uuid_ =
          boost::lexical_cast<std::string>(boost::uuids::random_generator()());
  }

  // Initialise mesh and particles
  virtual bool initialise_mesh() = 0;

  // Initialise particles
  virtual bool initialise_particles() = 0;

  // Initialise materials
  virtual bool initialise_materials() = 0;

  // Initialise external loads
  virtual bool initialise_loads() = 0;

  // Initialise math functions
  virtual bool initialise_math_functions(const Json&) = 0;

  //! Apply properties to particles sets (e.g: material)
  virtual bool apply_properties_to_particles_sets() = 0;

  // Solve
  virtual bool solve() = 0;

  // Check point restart
  virtual bool checkpoint_resume() = 0;

  //! Write HDF5 files
  virtual void write_hdf5(mpm::Index step, mpm::Index max_steps) = 0;

#ifdef USE_VTK
  //! Write VTK files
  virtual void write_vtk(mpm::Index step, mpm::Index max_steps) = 0;
#endif

 protected:
  //! A unique id for the analysis
  std::string uuid_;
  //! Time step size
  double dt_{std::numeric_limits<double>::max()};
  //! Current step
  mpm::Index step_{0};
  //! Number of steps
  mpm::Index nsteps_{std::numeric_limits<mpm::Index>::max()};
  //! Output steps
  mpm::Index output_steps_{std::numeric_limits<mpm::Index>::max()};
  //! A shared ptr to IO object
  std::shared_ptr<mpm::IO> io_;
  //! JSON analysis object
  Json analysis_;
  //! JSON post-process object
  Json post_process_;
  //! Logger
  std::shared_ptr<spdlog::logger> console_;
};
}  // namespace mpm

#endif  // MPM_MPM_H_
