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
#include <stratosphere.hpp>
#include "ro_nrr_utils.hpp"
#include "ro_service_impl.hpp"

namespace ams::ro::impl {

    namespace {

        constexpr size_t KeyGenerationMax100 = 0;
        constexpr size_t KeyGenerationMax910 = 1;
        constexpr size_t KeyGenerationMax    = std::max(KeyGenerationMax100, KeyGenerationMax910);
        constexpr size_t KeyGenerationCount  = KeyGenerationMax + 1;

        constexpr size_t RsaKeySize = 0x100;

        constexpr const u8 Exponent[3] = { 0x01, 0x00, 0x01 };

        constexpr const u8 DevModuli[KeyGenerationCount][RsaKeySize] = {
            {
                0xC1, 0x15, 0x7C, 0x02, 0x26, 0xE5, 0x35, 0x6F, 0x99, 0xDB, 0xBE, 0xBD, 0xD7, 0x01, 0x07, 0x1C,
                0xC2, 0x3D, 0x1E, 0x6B, 0x7E, 0x08, 0x07, 0xBC, 0xE2, 0x6D, 0x49, 0xEC, 0x0B, 0xFF, 0xE4, 0x91,
                0x8C, 0x62, 0xB9, 0xFC, 0x69, 0xBF, 0x3A, 0xB6, 0x6C, 0x3A, 0x5D, 0x0E, 0x31, 0x5E, 0x6C, 0x1D,
                0x9A, 0x21, 0xD9, 0x9D, 0xD3, 0xB8, 0x50, 0x5F, 0x27, 0x7C, 0x4A, 0xD2, 0xFE, 0xE8, 0xDA, 0x1C,
                0xB9, 0x9E, 0x7E, 0x1E, 0x2F, 0x7D, 0xF9, 0x70, 0xA2, 0x98, 0x19, 0x6A, 0x53, 0x40, 0x64, 0xE7,
                0xC0, 0x92, 0xAE, 0x64, 0xD2, 0x01, 0xB2, 0x49, 0x30, 0x19, 0x7F, 0xF8, 0x6E, 0x0D, 0x49, 0x35,
                0xE9, 0x95, 0x77, 0x00, 0x65, 0xC5, 0x1E, 0xF5, 0x2A, 0xF9, 0xA1, 0x52, 0xA0, 0xA4, 0xFA, 0x87,
                0x3D, 0x8F, 0x51, 0xEC, 0x02, 0x80, 0xA4, 0xC7, 0x22, 0x74, 0xEF, 0x56, 0x61, 0x71, 0x39, 0xE2,
                0x2F, 0x03, 0x82, 0xDB, 0x50, 0xE9, 0xCC, 0x60, 0x48, 0x46, 0x71, 0xE2, 0xC6, 0x71, 0xF3, 0xF9,
                0x85, 0x52, 0x1A, 0xE2, 0xA8, 0x18, 0x77, 0x86, 0xD0, 0x12, 0xEB, 0x4F, 0x81, 0xA0, 0xDF, 0x20,
                0x42, 0xF0, 0xF8, 0xE3, 0x00, 0xE6, 0xFC, 0xA7, 0x44, 0xF0, 0xDC, 0x2B, 0x5B, 0xA0, 0xD3, 0x01,
                0x34, 0xD0, 0xD7, 0xFD, 0xEF, 0x66, 0x92, 0xB3, 0x87, 0x64, 0xD9, 0x76, 0xDA, 0x6E, 0x3A, 0x19,
                0x98, 0x1F, 0xBD, 0x1F, 0x25, 0x69, 0x9F, 0x28, 0xE6, 0x9E, 0xB7, 0x38, 0x92, 0x12, 0x16, 0xDE,
                0xDA, 0xE2, 0xB9, 0x7E, 0xFA, 0x98, 0x94, 0xF4, 0x9A, 0xDF, 0x2D, 0xC0, 0x99, 0x83, 0x61, 0xAD,
                0xB8, 0x3E, 0x27, 0x3F, 0x0E, 0xB8, 0x9E, 0x9B, 0x11, 0x78, 0xF1, 0x06, 0x30, 0x5B, 0xCA, 0xF4,
                0xEB, 0x72, 0x20, 0xD3, 0x15, 0x15, 0xC0, 0xC7, 0x1A, 0x08, 0xAE, 0x6E, 0xB2, 0x02, 0x43, 0xE9,
            },
            {
                0xB0, 0x3A, 0xC3, 0x11, 0x58, 0xAC, 0x95, 0x9D, 0xED, 0x88, 0x80, 0xD9, 0x93, 0x71, 0x8E, 0xA0,
                0xBD, 0x19, 0x68, 0x6A, 0x06, 0x63, 0x7F, 0x06, 0x93, 0xBA, 0x43, 0x24, 0x5E, 0xD0, 0x54, 0x76,
                0x2F, 0x8D, 0x6A, 0xF4, 0x7B, 0x16, 0x7A, 0x68, 0xE2, 0xE6, 0x95, 0x82, 0x57, 0xD9, 0x01, 0x34,
                0x87, 0x42, 0x24, 0x83, 0x91, 0x7F, 0xE7, 0xB4, 0xB7, 0xE5, 0x8B, 0x4B, 0x08, 0x1F, 0x70, 0x58,
                0x5A, 0x30, 0xE0, 0xC9, 0xAB, 0xB8, 0xB1, 0x96, 0xFA, 0x45, 0xA2, 0xF7, 0x3C, 0x94, 0x65, 0x97,
                0xE2, 0x72, 0x7A, 0x19, 0xE2, 0x5F, 0x30, 0xF0, 0xA9, 0x44, 0xD8, 0x4B, 0x00, 0xEE, 0xD0, 0x99,
                0xA7, 0xAB, 0xC7, 0x14, 0x94, 0xE8, 0xD8, 0x70, 0x8F, 0xC7, 0x3C, 0x95, 0x0F, 0xE9, 0x52, 0x44,
                0x7A, 0xA4, 0xCA, 0xB4, 0xCE, 0x31, 0xCD, 0xC8, 0xC0, 0xF3, 0x08, 0x51, 0xA3, 0xC1, 0x5D, 0xA5,
                0x3E, 0x4A, 0xCE, 0xE8, 0x17, 0xA2, 0xED, 0xB8, 0x94, 0xD7, 0x3C, 0xF0, 0x42, 0x5A, 0xC1, 0x83,
                0xE8, 0x65, 0x28, 0x79, 0x43, 0x2C, 0x17, 0xBB, 0x68, 0x91, 0x80, 0xE3, 0xDA, 0x6B, 0xD4, 0x0B,
                0xA3, 0x6A, 0x8B, 0xDB, 0x1E, 0x2E, 0x16, 0xFE, 0xAB, 0x81, 0x16, 0x84, 0x6E, 0x53, 0xFD, 0xB6,
                0x1F, 0x12, 0xDE, 0x77, 0x79, 0xE6, 0x9A, 0x02, 0x95, 0x6B, 0x22, 0xFF, 0xE3, 0x10, 0xD2, 0x0D,
                0x5B, 0x33, 0xB1, 0xC0, 0x5C, 0xF6, 0xAC, 0x69, 0xAF, 0xCF, 0xD2, 0x34, 0x4B, 0x76, 0x88, 0xC1,
                0x91, 0x66, 0x6C, 0x2F, 0x06, 0xBA, 0x2B, 0xC7, 0x35, 0x0A, 0xD8, 0xD7, 0x5C, 0xF1, 0x20, 0x67,
                0x31, 0x4A, 0x9B, 0xB7, 0xCA, 0x4A, 0x3F, 0xAE, 0x4F, 0xF3, 0x02, 0x71, 0x97, 0xD6, 0x78, 0xC6,
                0x8E, 0x4B, 0x41, 0x78, 0x5B, 0x56, 0x6F, 0x6C, 0xA0, 0x0D, 0x83, 0x28, 0x25, 0x07, 0x75, 0x15,
            }
        };

        constexpr const u8 ProdModuli[KeyGenerationCount][RsaKeySize] = {
            {
                0xCF, 0x2F, 0x54, 0xF1, 0x7D, 0xA3, 0x11, 0x47, 0xC3, 0xBD, 0xED, 0x10, 0x3D, 0x64, 0xF8, 0x6B,
                0x51, 0x62, 0x05, 0x6F, 0x10, 0x5F, 0x50, 0x76, 0xF2, 0x40, 0x09, 0xA3, 0x16, 0xFC, 0xC3, 0xA8,
                0x75, 0xFD, 0x7C, 0xB4, 0x38, 0x9D, 0x30, 0x5C, 0x02, 0x6E, 0x2C, 0xCF, 0x03, 0x84, 0x23, 0x64,
                0x4A, 0xF9, 0x90, 0x44, 0x58, 0xAF, 0x43, 0x19, 0xD2, 0x27, 0xF1, 0x60, 0x28, 0xB8, 0x7B, 0x26,
                0xD3, 0x21, 0xE8, 0xF0, 0xED, 0x65, 0x01, 0x31, 0x51, 0x45, 0xD3, 0x93, 0xCF, 0x07, 0x65, 0xF1,
                0x8A, 0x72, 0x61, 0x55, 0x63, 0x91, 0x84, 0x63, 0x2C, 0x0B, 0x3E, 0xF6, 0x7E, 0x9B, 0x19, 0x4C,
                0x42, 0xE3, 0xB9, 0x83, 0xF3, 0xA1, 0x2B, 0x7B, 0xA1, 0x5F, 0x00, 0xDC, 0x80, 0x1A, 0x6D, 0x6C,
                0x22, 0x20, 0x2F, 0x19, 0x61, 0x96, 0x5E, 0xD9, 0x93, 0x45, 0x62, 0x48, 0xC1, 0xB1, 0x48, 0x8A,
                0x07, 0x6C, 0xBB, 0x3D, 0xCC, 0xE0, 0xEF, 0x43, 0x56, 0x1D, 0x2A, 0x1D, 0x8E, 0x19, 0xBF, 0x99,
                0x7A, 0x56, 0x9B, 0x72, 0xB9, 0x49, 0x86, 0xD4, 0x5D, 0xA0, 0xE5, 0x40, 0x2C, 0x2D, 0x31, 0x5A,
                0x68, 0x90, 0xA4, 0xDA, 0xED, 0x52, 0xCF, 0xE4, 0x3C, 0x35, 0x5E, 0x94, 0xC8, 0x36, 0x6E, 0x5F,
                0xF5, 0x50, 0x9D, 0xBD, 0x3F, 0x86, 0x28, 0xC4, 0xF7, 0x0D, 0xFA, 0x1D, 0x65, 0xCE, 0x7F, 0x83,
                0x8E, 0xEE, 0x75, 0x8B, 0x8D, 0xF3, 0x1C, 0x3E, 0x8B, 0xB7, 0x31, 0x60, 0x0E, 0x33, 0xC0, 0x8C,
                0xEF, 0xBA, 0xD0, 0xF0, 0x56, 0x35, 0x11, 0x70, 0x3D, 0x3A, 0x60, 0xFF, 0xBB, 0xC8, 0x49, 0xB3,
                0x50, 0x93, 0xB5, 0x0C, 0x4D, 0x62, 0x2A, 0x53, 0x76, 0x69, 0x5D, 0x81, 0x35, 0x79, 0x08, 0xAE,
                0x08, 0x7D, 0x51, 0x72, 0xE4, 0xE9, 0xD9, 0xBE, 0x91, 0xB9, 0x29, 0x89, 0xDD, 0xC9, 0xF9, 0xDB,
            },
            {
                0xBD, 0x8E, 0x75, 0x8D, 0x6A, 0x5E, 0xCF, 0x1A, 0x68, 0x7D, 0x9D, 0x06, 0x80, 0x60, 0x48, 0x56,
                0x73, 0x13, 0xB8, 0x8D, 0xA5, 0x83, 0x11, 0xE3, 0xF4, 0xB5, 0xBF, 0x21, 0x56, 0x50, 0x6F, 0x68,
                0xFD, 0x2E, 0xEF, 0x3A, 0xE1, 0x7D, 0x5D, 0xA2, 0x02, 0x21, 0xAD, 0x57, 0x7F, 0xA9, 0xD1, 0x95,
                0xA6, 0x5C, 0x80, 0x67, 0x2A, 0xAE, 0x29, 0xCD, 0x98, 0x4C, 0x71, 0x98, 0x22, 0xD3, 0x39, 0x77,
                0xA3, 0x39, 0xE8, 0xB0, 0x7E, 0x77, 0x4D, 0x54, 0xA5, 0x9F, 0xF3, 0xF1, 0xF0, 0x85, 0x74, 0x64,
                0xF7, 0x17, 0xAC, 0xEB, 0x95, 0xAD, 0x01, 0x3A, 0xA2, 0xFB, 0x18, 0xEC, 0x6C, 0x06, 0xD5, 0xBD,
                0x0F, 0x66, 0x99, 0x2C, 0xEE, 0x81, 0x87, 0x40, 0xDA, 0xBC, 0x30, 0xC9, 0x54, 0x21, 0x46, 0x6B,
                0xD5, 0xAE, 0xDF, 0xA8, 0x8F, 0x61, 0x19, 0x5E, 0x75, 0x68, 0xED, 0x80, 0x73, 0x5F, 0x4D, 0xF2,
                0xC2, 0x09, 0xC8, 0x2D, 0x73, 0x12, 0x18, 0x35, 0x7F, 0x5E, 0xA8, 0xAD, 0x37, 0x6B, 0x9C, 0x14,
                0x8C, 0x4E, 0x7D, 0xC0, 0xDC, 0x42, 0x45, 0xDD, 0x84, 0x5A, 0x0B, 0xED, 0x97, 0xBE, 0x66, 0xD1,
                0xC0, 0xC4, 0x7C, 0x48, 0x66, 0x22, 0xAA, 0xFB, 0x47, 0x95, 0x84, 0x45, 0x2E, 0x70, 0x35, 0x68,
                0x04, 0xF0, 0xBC, 0xEE, 0xCB, 0x6E, 0xB7, 0x09, 0xBA, 0xD9, 0xEA, 0xF9, 0x78, 0x32, 0x4C, 0xD5,
                0x0F, 0x6E, 0xEF, 0x4D, 0x21, 0xE0, 0xE7, 0xEA, 0x17, 0xA3, 0xEC, 0x0D, 0x76, 0xC0, 0xB9, 0xCC,
                0x1B, 0x8C, 0x7E, 0xFB, 0x56, 0x5B, 0x60, 0x94, 0x26, 0xB7, 0x9F, 0x2B, 0xAD, 0xC6, 0x1F, 0xBD,
                0xB6, 0xFA, 0x33, 0xDB, 0x1C, 0xBC, 0x28, 0xE1, 0x85, 0x1E, 0x2F, 0xF9, 0xFD, 0xEA, 0x83, 0x52,
                0x2F, 0xEE, 0x07, 0x7C, 0x1E, 0x2F, 0xCD, 0x76, 0x91, 0xEA, 0x27, 0xE6, 0x50, 0x9A, 0x86, 0x23,
            }
        };

        /* Helper functions. */

        Result GetCertificationModulus(const u8 **out, u32 key_generation) {
            if (hos::GetVersion() >= hos::Version_9_1_0) {
                R_UNLESS(key_generation <= KeyGenerationMax910, ro::ResultNotAuthorized());
            } else {
                R_UNLESS(key_generation <= KeyGenerationMax100, ro::ResultNotAuthorized());
            }

            *out = IsDevelopmentHardware() ? DevModuli[key_generation] : ProdModuli[key_generation];
            return ResultSuccess();
        }

        Result ValidateNrrCertification(const NrrHeader *header, const u8 *mod) {
            static_assert(RsaKeySize == NrrCertification::RsaKeySize);

            /* Verify the signature. */
            const u8 *sig         = header->GetCertificationSignature();
            const size_t sig_size = RsaKeySize;
            const size_t mod_size = RsaKeySize;
            const u8 *exp         = Exponent;
            const size_t exp_size = util::size(Exponent);
            const u8 *msg         = header->GetCertificationSignedArea();
            const size_t msg_size = NrrCertification::SignedSize;
            R_UNLESS(crypto::VerifyRsa2048PssSha256(sig, sig_size, mod, mod_size, exp, exp_size, msg, msg_size), ro::ResultNotAuthorized());

            /* Check ProgramId pattern is valid. */
            R_UNLESS(header->IsProgramIdValid(), ResultNotAuthorized());

            return ResultSuccess();
        }

        Result ValidateNrrSignature(const NrrHeader *header) {
            static_assert(RsaKeySize == NrrCertification::RsaKeySize);

            /* Verify the signature. */
            const u8 *sig         = header->GetSignature();
            const size_t sig_size = RsaKeySize;
            const u8 *mod         = header->GetCertificationModulus();
            const size_t mod_size = RsaKeySize;
            const u8 *exp         = Exponent;
            const size_t exp_size = util::size(Exponent);
            const u8 *msg         = header->GetSignedArea();
            const size_t msg_size = header->GetSignedAreaSize();
            R_UNLESS(crypto::VerifyRsa2048PssSha256(sig, sig_size, mod, mod_size, exp, exp_size, msg, msg_size), ro::ResultNotAuthorized());

            return ResultSuccess();
        }

        Result ValidateNrr(const NrrHeader *header, u64 size, ncm::ProgramId program_id, NrrKind nrr_kind, bool enforce_nrr_kind) {
            /* Check magic. */
            R_UNLESS(header->IsMagicValid(), ResultInvalidNrr());

            /* Check size. */
            R_UNLESS(header->GetSize() == size, ResultInvalidSize());

            /* Only perform checks if we must. */
            const bool ease_nro_restriction = ShouldEaseNroRestriction();

            /* Get certification modulus. */
            /* NOTE: This must succeed even when ease_nro_restriction is true. */
            const u8 *modulus;
            R_TRY(GetCertificationModulus(std::addressof(modulus), header->GetKeyGeneration()));

            if (!ease_nro_restriction) {
                /* Check certification signature. */
                R_TRY(ValidateNrrCertification(header, modulus));

                /* Check NRR signature. */
                R_TRY(ValidateNrrSignature(header));

                /* Check program id. */
                R_UNLESS(header->GetProgramId() == program_id, ResultInvalidNrr());

                /* Check nrr kind. */
                if (hos::GetVersion() >= hos::Version_7_0_0 && enforce_nrr_kind) {
                    R_UNLESS(header->GetNrrKind() == nrr_kind, ResultInvalidNrrKind());
                }
            }

            return ResultSuccess();
        }

    }

    /* Utilities for working with NRRs. */
    Result MapAndValidateNrr(NrrHeader **out_header, u64 *out_mapped_code_address, void *out_hash, size_t out_hash_size, os::NativeHandle process_handle, ncm::ProgramId program_id, u64 nrr_heap_address, u64 nrr_heap_size, NrrKind nrr_kind, bool enforce_nrr_kind) {
        map::MappedCodeMemory nrr_mcm(ResultInternalError{});

        /* First, map the NRR. */
        R_TRY(map::MapCodeMemoryInProcess(nrr_mcm, process_handle, nrr_heap_address, nrr_heap_size));

        const u64 code_address = nrr_mcm.GetDstAddress();
        uintptr_t map_address;
        R_UNLESS(R_SUCCEEDED(map::LocateMappableSpace(&map_address, nrr_heap_size)), ResultOutOfAddressSpace());

        /* Nintendo...does not check the return value of this map. We will check, instead of aborting if it fails. */
        map::AutoCloseMap nrr_map(map_address, process_handle, code_address, nrr_heap_size);
        R_TRY(nrr_map.GetResult());

        NrrHeader *nrr_header = reinterpret_cast<NrrHeader *>(map_address);
        R_TRY(ValidateNrr(nrr_header, nrr_heap_size, program_id, nrr_kind, enforce_nrr_kind));

        /* Invalidation here actually prevents them from unmapping at scope exit. */
        nrr_map.Invalidate();
        nrr_mcm.Invalidate();

        /* Save a copy of the hash that we verified. */
        crypto::GenerateSha256Hash(out_hash, out_hash_size, nrr_header->GetSignedArea(), nrr_header->GetSignedAreaSize());

        *out_header = nrr_header;
        *out_mapped_code_address = code_address;
        return ResultSuccess();
    }

    Result UnmapNrr(os::NativeHandle process_handle, const NrrHeader *header, u64 nrr_heap_address, u64 nrr_heap_size, u64 mapped_code_address) {
        R_TRY(svc::UnmapProcessMemory(reinterpret_cast<uintptr_t>(header), process_handle, mapped_code_address, nrr_heap_size));
        R_TRY(svc::UnmapProcessCodeMemory(process_handle, mapped_code_address, nrr_heap_address, nrr_heap_size));
        return ResultSuccess();
    }

    bool ValidateNrrHashTableEntry(const void *signed_area, size_t signed_area_size, size_t hashes_offset, size_t num_hashes, const void *nrr_hash, const u8 *hash_table, const void *desired_hash) {
        crypto::Sha256Generator sha256;
        sha256.Initialize();


        /* Hash data before the hash table. */
        const size_t pre_hash_table_size = hashes_offset - NrrHeader::GetSignedAreaOffset();
        sha256.Update(signed_area, pre_hash_table_size);

        /* Hash the hash table, checking if the desired hash exists inside it. */
        size_t remaining_size = signed_area_size - pre_hash_table_size;
        bool found_hash = false;
        for (size_t i = 0; i < num_hashes; i++) {
            /* Get the current hash. */
            u8 cur_hash[crypto::Sha256Generator::HashSize];
            std::memcpy(cur_hash, hash_table, sizeof(cur_hash));

            /* Hash the current hash. */
            sha256.Update(cur_hash, sizeof(cur_hash));

            /* Check if the current hash is our target. */
            found_hash |= std::memcmp(cur_hash, desired_hash, sizeof(cur_hash)) == 0;

            /* Advance our pointers. */
            hash_table     += sizeof(cur_hash);
            remaining_size -= sizeof(cur_hash);
        }

        /* Data after the hash table should be all zeroes. */
        u8 work_buf[crypto::Sha256Generator::HashSize];
        {
            crypto::ClearMemory(work_buf, sizeof(work_buf));
            while (remaining_size > 0) {
                const size_t cur_size = std::min(remaining_size, sizeof(work_buf));
                sha256.Update(work_buf, cur_size);
                remaining_size -= cur_size;
            }
        }

        /* Validate the final hash. */
        sha256.GetHash(work_buf, sizeof(work_buf));

        /* Use & operator to avoid short circuiting. */
        const bool is_valid = found_hash & (std::memcmp(work_buf, nrr_hash, sizeof(work_buf)) == 0);

        /* Return result. */
        return is_valid;
    }

}
