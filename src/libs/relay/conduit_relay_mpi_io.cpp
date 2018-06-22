//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
// Copyright (c) 2014-2018, Lawrence Livermore National Security, LLC.
// 
// Produced at the Lawrence Livermore National Laboratory
// 
// LLNL-CODE-666778
// 
// All rights reserved.
// 
// This file is part of Conduit. 
// 
// For details, see: http://software.llnl.gov/conduit/.
// 
// Please also read conduit/LICENSE
// 
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions are met:
// 
// * Redistributions of source code must retain the above copyright notice, 
//   this list of conditions and the disclaimer below.
// 
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the disclaimer (as noted below) in the
//   documentation and/or other materials provided with the distribution.
// 
// * Neither the name of the LLNS/LLNL nor the names of its contributors may
//   be used to endorse or promote products derived from this software without
//   specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL LAWRENCE LIVERMORE NATIONAL SECURITY,
// LLC, THE U.S. DEPARTMENT OF ENERGY OR CONTRIBUTORS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
// DAMAGES  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
// OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
// IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
// POSSIBILITY OF SUCH DAMAGE.
// 
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

//-----------------------------------------------------------------------------
///
/// file: conduit_relay_mpi_io.cpp
///
//-----------------------------------------------------------------------------

#include "conduit_relay_mpi_io.hpp"

//-----------------------------------------------------------------------------
// standard lib includes
//-----------------------------------------------------------------------------
#include <iostream>



// includes for optional features
#ifdef CONDUIT_RELAY_IO_HDF5_ENABLED
#include "conduit_relay_mpi_io_hdf5.hpp"
#endif

#ifdef CONDUIT_RELAY_IO_SILO_ENABLED
#include "conduit_relay_mpi_io_silo.hpp"
#endif

#ifdef CONDUIT_RELAY_IO_ADIOS_ENABLED
#include "conduit_relay_mpi_io_adios.hpp"
#endif

//-----------------------------------------------------------------------------
// -- begin conduit:: --
//-----------------------------------------------------------------------------
namespace conduit
{

//-----------------------------------------------------------------------------
// -- begin conduit::relay --
//-----------------------------------------------------------------------------
namespace relay
{

//-----------------------------------------------------------------------------
// -- begin conduit::relay::mpi --
//-----------------------------------------------------------------------------
namespace mpi
{

//-----------------------------------------------------------------------------
// -- begin conduit::relay::mpi::io --
//-----------------------------------------------------------------------------
namespace io
{

// Include a helper function for figuring out protocols.
#include "conduit_relay_identify_protocol.hpp"

//---------------------------------------------------------------------------//
void 
save(const Node &node,
     const std::string &path,
     MPI_Comm comm)
{
    std::string protocol;
    identify_protocol(path,protocol);
    save(node,path,protocol,comm);
}

//---------------------------------------------------------------------------//
void 
save_merged(const Node &node,
            const std::string &path,
            MPI_Comm comm)
{
    std::string protocol;
    identify_protocol(path,protocol);
    save_merged(node,path,protocol,comm);
}

//---------------------------------------------------------------------------//
void 
load(const std::string &path,
     Node &node,
     MPI_Comm comm)
{
    std::string protocol;
    identify_protocol(path,protocol);
    load(path,protocol,node,comm);
}

//---------------------------------------------------------------------------//
void 
load_merged(const std::string &path,
            Node &node,
            MPI_Comm comm)
{
    std::string protocol;
    identify_protocol(path,protocol);
    load_merged(path,protocol,node,comm);
}

//---------------------------------------------------------------------------//
void 
save(const Node &node,
     const std::string &path,
     const std::string &protocol,
     MPI_Comm comm)
{
    Node options;
    save(node, path, protocol, options, comm);
}

//---------------------------------------------------------------------------//
void 
save(const Node &node,
     const std::string &path,
     const std::string &protocol,
     const Node &options,
     MPI_Comm comm)
{
    // support conduit::Node's basic save cases
    if(protocol == "conduit_bin" ||
       protocol == "json" || 
       protocol == "conduit_json" ||
       protocol == "conduit_base64_json" )
    {
        node.save(path,protocol);
    }
    else if( protocol == "hdf5")
    {
#ifdef CONDUIT_RELAY_IO_HDF5_ENABLED
        hdf5_set_options(options);
        hdf5_write(node,path);
#else
        CONDUIT_ERROR("conduit_relay lacks HDF5 support: " << 
                      "Failed to save conduit node to path " << path);
#endif
    }
    else if( protocol == "conduit_silo")
    {
#ifdef CONDUIT_RELAY_IO_SILO_ENABLED
        silo_write(node,path);
#else
        CONDUIT_ERROR("conduit_relay lacks Silo support: " << 
                      "Failed to save conduit node to path " << path);
#endif
    }
    else if(protocol == "conduit_silo_mesh")
    {
#ifdef CONDUIT_RELAY_IO_SILO_ENABLED
        silo_mesh_write(node,path);
#else
        CONDUIT_ERROR("conduit_relay lacks Silo support: " << 
                      "Failed to save conduit mesh node to path " << path);
#endif
    }
    else if( protocol == "adios")
    {
#ifdef CONDUIT_RELAY_IO_ADIOS_ENABLED
        Node save_options;
        adios_options(save_options);

        adios_set_options(options);
        adios_save(node,path,comm);

        adios_set_options(save_options);
#else
        CONDUIT_ERROR("conduit_relay lacks ADIOS support: " << 
                      "Failed to save conduit node to path " << path);
#endif
    }
    else
    {
        CONDUIT_ERROR("unknown conduit_relay protocol: " << protocol);
    }
}

//---------------------------------------------------------------------------//
void 
save_merged(const Node &node,
            const std::string &path,
            const std::string &protocol,
            MPI_Comm comm)
{
    Node options;
    save_merged(node, path, protocol, options, comm);
}

//---------------------------------------------------------------------------//
void 
save_merged(const Node &node,
            const std::string &path,
            const std::string &protocol,
            const Node &options,
            MPI_Comm comm)
{
    // support conduit::Node's basic save cases
    if(protocol == "conduit_bin" ||
       protocol == "json" || 
       protocol == "conduit_json" ||
       protocol == "conduit_base64_json" )
    {
        Node n;
        n.load(path,protocol);
        n.update(node);
        n.save(path,protocol);
    }
    else if( protocol == "hdf5")
    {
#ifdef CONDUIT_RELAY_IO_HDF5_ENABLED
        hdf5_set_options(options);
        hdf5_append(node,path);
#else
        CONDUIT_ERROR("conduit_relay lacks HDF5 support: " << 
                      "Failed to save conduit node to path " << path);
#endif
    }
    else if( protocol == "conduit_silo")
    {
#ifdef CONDUIT_RELAY_IO_SILO_ENABLED
        Node n;
        silo_read(path,n);
        n.update(node);
        silo_write(n,path);
#else
        CONDUIT_ERROR("conduit_relay lacks Silo support: " << 
                      "Failed to save conduit node to path " << path);
#endif
    }
    else if(protocol == "conduit_silo_mesh")
    {
#ifdef CONDUIT_RELAY_IO_SILO_ENABLED
        /// TODO .. ?
        silo_mesh_write(node,path);
#else
        CONDUIT_ERROR("conduit_relay lacks Silo support: " << 
                      "Failed to save conduit mesh node to path " << path);
#endif
    }
    else if( protocol == "adios")
    {
#ifdef CONDUIT_RELAY_IO_ADIOS_ENABLED
        Node save_options;
        adios_options(save_options);

        adios_set_options(options);
        adios_save_merged(node,path,comm);

        adios_set_options(save_options);
#else
        CONDUIT_ERROR("conduit_relay lacks ADIOS support: " << 
                      "Failed to save conduit node to path " << path);
#endif
    }
    else
    {
        CONDUIT_ERROR("unknown conduit_relay protocol: " << protocol);
    }
}


//---------------------------------------------------------------------------//
void
load(const std::string &path,
     const std::string &protocol,
     Node &options,
     Node &node,
     MPI_Comm comm)
{

    // support conduit::Node's basic load cases
    if(protocol == "conduit_bin" ||
       protocol == "json" || 
       protocol == "conduit_json" ||
       protocol == "conduit_base64_json" )
    {
        node.load(path,protocol);
    }
    else if( protocol == "hdf5")
    {
#ifdef CONDUIT_RELAY_IO_HDF5_ENABLED
        node.reset();
        hdf5_read(path,node);
#else
        CONDUIT_ERROR("conduit_relay lacks HDF5 support: " << 
                      "Failed to load conduit node from path " << path);
#endif
    }
    else if( protocol == "conduit_silo")
    {
#ifdef CONDUIT_RELAY_IO_SILO_ENABLED
        silo_read(path,node);
#else
        CONDUIT_ERROR("conduit_relay lacks Silo support: " << 
                    "Failed to load conduit node from path " << path);
#endif
    }
    else if(protocol == "conduit_silo_mesh")
    {
        CONDUIT_ERROR("the conduit_relay conduit_silo_mesh protocol does not "
                      "support \"load\"");
    }
    else if( protocol == "adios")
    {
#ifdef CONDUIT_RELAY_IO_ADIOS_ENABLED
        Node load_options;
        adios_options(load_options);
        adios_set_options(options);

        node.reset();
        adios_load(path,node,comm);

        adios_set_options(load_options);
#else
        CONDUIT_ERROR("conduit_relay lacks ADIOS support: " << 
                    "Failed to load conduit node from path " << path);
#endif
    }
    else
    {
        CONDUIT_ERROR("unknown conduit_relay protocol: " << protocol);
        
    }
}

//---------------------------------------------------------------------------//
void
load(const std::string &path,
     const std::string &protocol,
     Node &node,
     MPI_Comm comm)
{
    Node options;
    load(path, protocol, options, node, comm);
}

//---------------------------------------------------------------------------//
void
load(const std::string &path,
     const std::string &protocol,
     int timestep,
     int domain,
     Node &options,
     Node &node,
     MPI_Comm comm)
{

    // support conduit::Node's basic load cases
    if(protocol == "conduit_bin" ||
       protocol == "json" || 
       protocol == "conduit_json" ||
       protocol == "conduit_base64_json" )
    {
        node.load(path,protocol);
    }
    else if( protocol == "hdf5")
    {
#ifdef CONDUIT_RELAY_IO_HDF5_ENABLED
        node.reset();
        hdf5_read(path,node);
#else
        CONDUIT_ERROR("conduit_relay lacks HDF5 support: " << 
                      "Failed to load conduit node from path " << path);
#endif
    }
    else if( protocol == "conduit_silo")
    {
#ifdef CONDUIT_RELAY_IO_SILO_ENABLED
        silo_read(path,node);
#else
        CONDUIT_ERROR("conduit_relay lacks Silo support: " << 
                    "Failed to load conduit node from path " << path);
#endif
    }
    else if(protocol == "conduit_silo_mesh")
    {
        CONDUIT_ERROR("the conduit_relay conduit_silo_mesh protocol does not "
                      "support \"load\"");
    }
    else if( protocol == "adios")
    {
#ifdef CONDUIT_RELAY_IO_ADIOS_ENABLED
        Node load_options;
        adios_options(load_options);
        adios_set_options(options);

        node.reset();
        adios_load(path,timestep,domain,node,comm);

        adios_set_options(load_options);
#else
        CONDUIT_ERROR("conduit_relay lacks ADIOS support: " << 
                    "Failed to load conduit node from path " << path);
#endif
    }
    else
    {
        CONDUIT_ERROR("unknown conduit_relay protocol: " << protocol);
        
    }
}

//---------------------------------------------------------------------------//
void
load(const std::string &path,
     const std::string &protocol,
     int timestep,
     int domain,
     Node &node,
     MPI_Comm comm)
{
    Node options;
    load(path, protocol, timestep, domain, options, node, comm);
}

//---------------------------------------------------------------------------//
void
load_merged(const std::string &path,
            const std::string &protocol,
            Node &node,
            MPI_Comm comm)
{
    // support conduit::Node's basic load cases
    if(protocol == "conduit_bin" ||
       protocol == "json" || 
       protocol == "conduit_json" ||
       protocol == "conduit_base64_json" )
    {
        Node n;
        n.load(path,protocol);
        // update into dest
        node.update(n);

    }
    else if( protocol == "hdf5")
    {
#ifdef CONDUIT_RELAY_IO_HDF5_ENABLED
        Node n;
        hdf5_read(path,n);
        node.update(n);
#else
        CONDUIT_ERROR("relay lacks HDF5 support: " << 
                      "Failed to read conduit node from path " << path);
#endif
    }
    else if( protocol == "conduit_silo")
    {
#ifdef CONDUIT_RELAY_IO_SILO_ENABLED
        Node n;
        silo_read(path,n);
        node.update(n);
#else
        CONDUIT_ERROR("relay lacks Silo support: " << 
                    "Failed to load conduit node from path " << path);
#endif
    }
    else if(protocol == "conduit_silo_mesh")
    {
        CONDUIT_ERROR("the relay conduit_silo_mesh protocol does not "
                      "support \"load\"");
    }
    else if( protocol == "adios")
    {
#ifdef CONDUIT_RELAY_IO_ADIOS_ENABLED
        Node n;
        adios_load(path,n,comm);
        node.update(n);
#else
        CONDUIT_ERROR("relay lacks ADIOS support: " << 
                      "Failed to read conduit node from path " << path);
#endif
    }
    else
    {
        CONDUIT_ERROR("relay unknown protocol: " << protocol);
        
    }

}

//-----------------------------------------------------------------------------
int query_number_of_domains(const std::string &path, MPI_Comm comm)
{
    int ndoms = 1;
    std::string protocol;
    identify_protocol(path,protocol);

    if(protocol == "adios")
    {
#ifdef CONDUIT_RELAY_IO_ADIOS_ENABLED
        ndoms = adios_query_number_of_domains(path, comm);
#endif
    }

    return ndoms;
}


}
//-----------------------------------------------------------------------------
// -- end conduit::relay::mpi::io --
//-----------------------------------------------------------------------------

}
//-----------------------------------------------------------------------------
// -- end conduit::relay::mpi --
//-----------------------------------------------------------------------------

}
//-----------------------------------------------------------------------------
// -- end conduit::relay --
//-----------------------------------------------------------------------------

}
//-----------------------------------------------------------------------------
// -- end conduit:: --
//-----------------------------------------------------------------------------

