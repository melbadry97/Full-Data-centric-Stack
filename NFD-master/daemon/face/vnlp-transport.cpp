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

#include "vnlp-transport.hpp"

namespace nfd {
namespace face {

NFD_LOG_INIT(VNLPTransport);

VNLPTransport::VNLPTransport(const FaceUri& localUri, const FaceUri& remoteUri,
                             ndn::nfd::FaceScope scope, ndn::nfd::FacePersistency persistency)
{
  this->setLocalUri(localUri);
  this->setRemoteUri(remoteUri);
  this->setScope(scope);
  this->setPersistency(persistency);
  this->setLinkType(ndn::nfd::LINK_TYPE_POINT_TO_POINT);
  this->setMtu(MTU_UNLIMITED);

  NFD_LOG_FACE_DEBUG("Creating VNLP transport");
}

} // namespace face
} // namespace nfd
