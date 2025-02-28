/*
 * Copyright (c) Atmosphère-NX
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#pragma once
#include <vapours.hpp>
#include <stratosphere/erpt/erpt_types.hpp>
#include <stratosphere/erpt/sf/erpt_sf_i_report.hpp>
#include <stratosphere/erpt/sf/erpt_sf_i_manager.hpp>
#include <stratosphere/erpt/sf/erpt_sf_i_attachment.hpp>

#define AMS_ERPT_I_SESSION_INTERFACE_INFO(C, H)                                                                                                        \
    AMS_SF_METHOD_INFO(C, H,  0, Result, OpenReport,     (ams::sf::Out<ams::sf::SharedPointer<erpt::sf::IReport>>     out), (out))                     \
    AMS_SF_METHOD_INFO(C, H,  1, Result, OpenManager,    (ams::sf::Out<ams::sf::SharedPointer<erpt::sf::IManager>>    out), (out))                     \
    AMS_SF_METHOD_INFO(C, H,  2, Result, OpenAttachment, (ams::sf::Out<ams::sf::SharedPointer<erpt::sf::IAttachment>> out), (out), hos::Version_8_0_0)

AMS_SF_DEFINE_INTERFACE(ams::erpt::sf, ISession, AMS_ERPT_I_SESSION_INTERFACE_INFO)
