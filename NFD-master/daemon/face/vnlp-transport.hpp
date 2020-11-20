/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2020 Mohammed Elbadry <mohammed.elbadry@stonybrook.edu> 
 *
 * This file is part of NFD (Named Data Networking Forwarding Daemon).
 * See AUTHORS.md for complete list of NFD authors and contributors.
 *
 * NFD is free software: you can redistribute it and/or modify it under the terms
 * of the GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * NFD is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * NFD, e.g., in COPYING.md file.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef NFD_DAEMON_FACE_VNLP_TRANSPORT_HPP
#define NFD_DAEMON_FACE_VNLP_TRANSPORT_HPP

#include "transport.hpp"
#include "vmac-usrsp.h"
namespace nfd {
namespace face {

/** \brief A Transport that drops every packet.
 */
class VNLPTransport FINAL_UNLESS_WITH_TESTS : public Transport
{
public:
  explicit
  VNLPTransport(const FaceUri& localUri = FaceUri("vnlp://"),
                const FaceUri& remoteUri = FaceUri("vnlp://"),
                ndn::nfd::FaceScope scope = ndn::nfd::FACE_SCOPE_NON_LOCAL,
                ndn::nfd::FacePersistency persistency = ndn::nfd::FACE_PERSISTENCY_PERMANENT);

protected:
  void
  doClose() OVERRIDE_WITH_TESTS_ELSE_FINAL
  {
    setState(TransportState::CLOSED);
  }


  virtual
   void
  doSend(int type, uint16_t seq, const Block& packet, const EndpointId&) OVERRIDE_WITH_TESTS_ELSE_FINAL
  {
  	send_vmac(type, 0, seq, boost::asio::buffer_cast<char*>(packet), strlen(boost::asio::buffer_cast<char*>(packet)), boost::asio::buffer_cast<char*>(packet.getName()), strlen(boost::asio::buffer_cast<char*>(packet.getName())))
  }
};

} // namespace face
} // namespace nfd

#endif // NFD_DAEMON_FACE_VMAC_TRANSPORT_HPP
