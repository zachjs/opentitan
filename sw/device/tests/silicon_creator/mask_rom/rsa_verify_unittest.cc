// Copyright lowRISC contributors.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

#include "sw/device/silicon_creator/mask_rom/rsa_verify.h"

#include <unordered_set>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace rsa_verify_unittest {
namespace {

TEST(MontMul, SmallNumbers) {
  // Values are from ex. 14.35 in Handbook of Applied Cryptography except for m'
  // and expected result are computed for base 2^32 and R = 2^3072 (96 base 2^32
  // digits):
  // - m' = -m^-1 mod b = 3837733825
  // - R^-1 mod m = 72136
  // - x * y * R^-1 mod m = 55123
  constexpr sigverify_rsa_buffer_t x{5792};
  constexpr sigverify_rsa_buffer_t y{1229};
  constexpr sigverify_rsa_key_t key{
      .n = {72639},
      .n0_inv = 3837733825,
      .exponent = 3,
  };
  constexpr sigverify_rsa_buffer_t exp_res{55123};
  // Uninitialized on purpose.
  sigverify_rsa_buffer_t act_res;

  mont_mul(&key, &x, &y, &act_res);

  EXPECT_THAT(exp_res.data, ::testing::ElementsAreArray(act_res.data));
}

TEST(MontMul, LargeNumbers) {
  constexpr sigverify_rsa_buffer_t sig{
      0xceb7e983, 0xe693b200, 0xf9153989, 0xcf899599, 0x1ec09fae, 0xf2f88007,
      0x2a24eed5, 0x9c5b7c4e, 0x21a153b2, 0xaf7583ae, 0x04fdd694, 0x7550094b,
      0xb2a69ac4, 0xe49d8022, 0x7ed6f162, 0x14bb3a1b, 0xbb29d8dd, 0x5c5815c2,
      0x7a80d848, 0xb122f449, 0x59dca808, 0xbc1443e2, 0xe304ff93, 0xcc97ee4b,
      0x42ef6b57, 0x1436839f, 0xae860b45, 0x6a843a17, 0x2381fb91, 0x09fd0635,
      0xa431aac3, 0xd7220269, 0xdf3e2697, 0x35e2915e, 0xedba6956, 0x1d387448,
      0x930006df, 0x961e5f00, 0xf2a7e960, 0x884e4add, 0x7dfe76b1, 0x4079aa79,
      0x1f3a378d, 0x96c20697, 0x268aea57, 0x2c8569a4, 0x0474f512, 0x2388555c,
      0x58679953, 0xe73da3a0, 0x43431b9a, 0x699f04d3, 0xfc0be066, 0xcce606f2,
      0xd94cdfa0, 0x6c1ddca3, 0xe96c11f6, 0xfc635db4, 0x3bdb4f69, 0xa621c3e7,
      0x9f292111, 0xb86e1e6b, 0xb74f923b, 0x592967a0, 0xc412097f, 0x8c1c8ca7,
      0x494fcdb6, 0x87c5fe0f, 0x50c01aee, 0x8a26368e, 0xeaf12232, 0x7dade4d8,
      0x39eb2ac6, 0x744f8aaa, 0xf34908ca, 0x1e0c656c, 0xe96d4e29, 0x8575d194,
      0xe439bd31, 0xa74a77e3, 0x0f465b88, 0xf4e21152, 0x80400ad8, 0xe58501ec,
      0xa29d7536, 0x55c19326, 0x9ebbc63e, 0x20c75aee, 0xef6783d7, 0x59ffdba5,
      0x879b937b, 0x43a5c74c, 0x82b8f825, 0xfdf04b3a, 0x8fc62fbe, 0x114e6da5,
  };
  constexpr sigverify_rsa_buffer_t exp_res{
      0x7603d6bd, 0xe7714c93, 0x97cf4c0a, 0xa2f79f2b, 0xd6d9a47a, 0x666d8e1d,
      0x298e418a, 0xdbb713ec, 0x84d319fd, 0x1e89e6c3, 0xa4671f40, 0xc324a13b,
      0x528c47d0, 0xa4dd82ef, 0x07d00906, 0xa42d7bc9, 0xdaa7dabb, 0x7833ef95,
      0x8bd072e1, 0x79cc5b32, 0x0acce463, 0x72341597, 0x48771929, 0x7ed99ffe,
      0x16c6323b, 0x79dfb968, 0xf0a209fe, 0x7f92c8aa, 0x0f1588aa, 0xc83fdf6f,
      0x5711982a, 0x2dae9583, 0x7cb22468, 0x646a7389, 0x5dde7838, 0x3d7bfa5e,
      0xf044da0c, 0xe18031a3, 0x41f219eb, 0x137f03e7, 0xeee7e63b, 0x0ca6e484,
      0x7a7834fd, 0x9dc43ffe, 0x2e577301, 0xf35042b3, 0x6617b57c, 0xda6b7dc4,
      0xf8a6dd5b, 0x67059290, 0x86866edc, 0x094355af, 0xc7989f1a, 0x10a9d520,
      0xda9ee036, 0x7e54f825, 0xbcd4f122, 0xe3e10de1, 0xed1d7412, 0xd84936df,
      0x2afb47dd, 0xee084c01, 0xf3dc8101, 0x56e4539c, 0xf16a855a, 0x3e09bf8f,
      0xa01e4b9d, 0x09791f8b, 0x94e0ffc6, 0x698b0428, 0x066418c3, 0x418bf9fa,
      0x9e07ba2a, 0xf5487bfd, 0x7e4ddea5, 0x31086807, 0x4c4cc5d9, 0xb4e58126,
      0x9bb65989, 0xf88824e9, 0xd97fb609, 0xa4cafb2e, 0x932e86d1, 0x59f0d7d2,
      0x71df95b3, 0x083ec525, 0x0759cea9, 0xe487335d, 0xbeb42a5c, 0x7dc33154,
      0x59e45235, 0x11ca6d14, 0x43334239, 0xfe7cd561, 0xb161f750, 0xafcef8de,
  };
  // Uninitialized on purpose.
  sigverify_rsa_buffer_t act_res;

  mont_mul(&kSigVerifyRsaKeys[0], &sig, &sig, &act_res);

  EXPECT_THAT(act_res.data, ::testing::ElementsAreArray(exp_res.data));
}

TEST(ModExp, BadExp) {
  // Exponent = 0
  constexpr sigverify_rsa_key_t bad_key{};
  sigverify_rsa_buffer_t empty;

  EXPECT_EQ(sigverify_mod_exp_ibex(&bad_key, &empty, &empty), false);
}

TEST(ModExp, Exp65537) {
  constexpr sigverify_rsa_buffer_t sig{
      0xceb7e983, 0xe693b200, 0xf9153989, 0xcf899599, 0x1ec09fae, 0xf2f88007,
      0x2a24eed5, 0x9c5b7c4e, 0x21a153b2, 0xaf7583ae, 0x04fdd694, 0x7550094b,
      0xb2a69ac4, 0xe49d8022, 0x7ed6f162, 0x14bb3a1b, 0xbb29d8dd, 0x5c5815c2,
      0x7a80d848, 0xb122f449, 0x59dca808, 0xbc1443e2, 0xe304ff93, 0xcc97ee4b,
      0x42ef6b57, 0x1436839f, 0xae860b45, 0x6a843a17, 0x2381fb91, 0x09fd0635,
      0xa431aac3, 0xd7220269, 0xdf3e2697, 0x35e2915e, 0xedba6956, 0x1d387448,
      0x930006df, 0x961e5f00, 0xf2a7e960, 0x884e4add, 0x7dfe76b1, 0x4079aa79,
      0x1f3a378d, 0x96c20697, 0x268aea57, 0x2c8569a4, 0x0474f512, 0x2388555c,
      0x58679953, 0xe73da3a0, 0x43431b9a, 0x699f04d3, 0xfc0be066, 0xcce606f2,
      0xd94cdfa0, 0x6c1ddca3, 0xe96c11f6, 0xfc635db4, 0x3bdb4f69, 0xa621c3e7,
      0x9f292111, 0xb86e1e6b, 0xb74f923b, 0x592967a0, 0xc412097f, 0x8c1c8ca7,
      0x494fcdb6, 0x87c5fe0f, 0x50c01aee, 0x8a26368e, 0xeaf12232, 0x7dade4d8,
      0x39eb2ac6, 0x744f8aaa, 0xf34908ca, 0x1e0c656c, 0xe96d4e29, 0x8575d194,
      0xe439bd31, 0xa74a77e3, 0x0f465b88, 0xf4e21152, 0x80400ad8, 0xe58501ec,
      0xa29d7536, 0x55c19326, 0x9ebbc63e, 0x20c75aee, 0xef6783d7, 0x59ffdba5,
      0x879b937b, 0x43a5c74c, 0x82b8f825, 0xfdf04b3a, 0x8fc62fbe, 0x114e6da5,
  };
  constexpr sigverify_rsa_buffer_t exp_res{
      0x55555555, 0x55555555, 0x55555555, 0x55555555, 0x55555555, 0x55555555,
      0x55555555, 0x55555555, 0x55555555, 0x55555555, 0x55555555, 0x55555555,
      0x55555555, 0x55555555, 0x55555555, 0x55555555, 0x55555555, 0x55555555,
      0x55555555, 0x55555555, 0x55555555, 0x55555555, 0x55555555, 0x55555555,
      0x55555555, 0x55555555, 0x55555555, 0x55555555, 0x55555555, 0x55555555,
      0x55555555, 0x55555555, 0x55555555, 0x55555555, 0x55555555, 0x55555555,
      0x55555555, 0x55555555, 0x55555555, 0x55555555, 0x55555555, 0x55555555,
      0x55555555, 0x55555555, 0x55555555, 0x55555555, 0x55555555, 0x55555555,
      0x55555555, 0x55555555, 0x55555555, 0x55555555, 0x55555555, 0x55555555,
      0x55555555, 0x55555555, 0x55555555, 0x55555555, 0x55555555, 0x55555555,
      0x55555555, 0x55555555, 0x55555555, 0x55555555, 0x55555555, 0x55555555,
      0x55555555, 0x55555555, 0x55555555, 0x55555555, 0x55555555, 0x55555555,
      0x55555555, 0x55555555, 0x55555555, 0x55555555, 0x55555555, 0x55555555,
      0x55555555, 0x55555555, 0x55555555, 0x55555555, 0x55555555, 0x55555555,
      0x55555555, 0x55555555, 0x55555555, 0x55555555, 0x55555555, 0x55555555,
      0x55555555, 0x55555555, 0x55555555, 0x55555555, 0x55555555, 0x55555555,
  };
  // Uninitialized on purpose.
  sigverify_rsa_buffer_t act_res;

  EXPECT_EQ(sigverify_mod_exp_ibex(&kSigVerifyRsaKeys[0], &sig, &act_res),
            true);
  EXPECT_THAT(act_res.data, ::testing::ElementsAreArray(exp_res.data));
}

TEST(ModExp, Exp3) {
  constexpr sigverify_rsa_buffer_t sig{
      0xca60cb6e, 0xd3a786de, 0x37623afb, 0x7f6cbc1a, 0x5366d957, 0xe69ffe2f,
      0x3560dc40, 0xf8b205c5, 0xa612764f, 0xb0415cf7, 0x5c5b87d9, 0xe0081c67,
      0xadc8d9bd, 0xdd072b18, 0x8e22b48b, 0x758b9df3, 0xb208d5ab, 0x5f1bcb08,
      0xb16f9e88, 0xf2d37daf, 0xf7fc6ecb, 0x5a102bb9, 0xe61a9b3f, 0x96541e3e,
      0x3718d3ef, 0x769b35c2, 0xf571f77a, 0x82a6f325, 0xaa5ef30c, 0x17048840,
      0xc71ddc21, 0xc7bc71ed, 0x14f7ed74, 0x7a3d6a80, 0xebb0c73d, 0x5f28b2c2,
      0x56a502db, 0x39a3814f, 0x37df9ba4, 0x397e700d, 0x6c03a24d, 0x9efa0232,
      0x257c4b77, 0xa928a03a, 0x43455edd, 0x57e509fb, 0x32458a00, 0x09941f22,
      0xca9af629, 0x76a01068, 0x4c638cbe, 0x9b8a3ff6, 0x3979752d, 0x6d4c9bb8,
      0x3ee6189c, 0xab9b7212, 0x2116de70, 0x429344c4, 0x072412c7, 0x747fff3d,
      0x9d07cc3f, 0xb188d846, 0xcf4959ed, 0xc622387c, 0x32aeb26b, 0xf2921d28,
      0x60793032, 0x061e4108, 0x6d70682b, 0x062ffa0b, 0x2b1f1696, 0x507a26f5,
      0x1401059c, 0x202485a9, 0xfe963ae9, 0x54423a9d, 0x9731f9bb, 0x227e9788,
      0x50846b54, 0x1e9f59f1, 0x3f158119, 0xc36f0b7b, 0xbef3b349, 0xf9172b6f,
      0x3daf21c0, 0x819ee37e, 0x0bba9299, 0x90727884, 0xc74908f9, 0xec095a40,
      0x8e2120dd, 0xfbefd497, 0x2227f721, 0xb7abdc98, 0xf7e55656, 0x3be75b5c,
  };
  constexpr sigverify_rsa_buffer_t exp_res{
      0x55555555, 0x55555555, 0x55555555, 0x55555555, 0x55555555, 0x55555555,
      0x55555555, 0x55555555, 0x55555555, 0x55555555, 0x55555555, 0x55555555,
      0x55555555, 0x55555555, 0x55555555, 0x55555555, 0x55555555, 0x55555555,
      0x55555555, 0x55555555, 0x55555555, 0x55555555, 0x55555555, 0x55555555,
      0x55555555, 0x55555555, 0x55555555, 0x55555555, 0x55555555, 0x55555555,
      0x55555555, 0x55555555, 0x55555555, 0x55555555, 0x55555555, 0x55555555,
      0x55555555, 0x55555555, 0x55555555, 0x55555555, 0x55555555, 0x55555555,
      0x55555555, 0x55555555, 0x55555555, 0x55555555, 0x55555555, 0x55555555,
      0x55555555, 0x55555555, 0x55555555, 0x55555555, 0x55555555, 0x55555555,
      0x55555555, 0x55555555, 0x55555555, 0x55555555, 0x55555555, 0x55555555,
      0x55555555, 0x55555555, 0x55555555, 0x55555555, 0x55555555, 0x55555555,
      0x55555555, 0x55555555, 0x55555555, 0x55555555, 0x55555555, 0x55555555,
      0x55555555, 0x55555555, 0x55555555, 0x55555555, 0x55555555, 0x55555555,
      0x55555555, 0x55555555, 0x55555555, 0x55555555, 0x55555555, 0x55555555,
      0x55555555, 0x55555555, 0x55555555, 0x55555555, 0x55555555, 0x55555555,
      0x55555555, 0x55555555, 0x55555555, 0x55555555, 0x55555555, 0x55555555,
  };
  // Uninitialized on purpose.
  sigverify_rsa_buffer_t act_res;

  EXPECT_EQ(sigverify_mod_exp_ibex(&kSigVerifyRsaKeys[1], &sig, &act_res),
            true);
  EXPECT_THAT(act_res.data, ::testing::ElementsAreArray(exp_res.data));
}

TEST(Keys, UniqueIds) {
  std::unordered_set<uint32_t> ids;
  for (auto const &key : kSigVerifyRsaKeys) {
    ids.insert(sigverify_rsa_key_id_get(&key));
  }

  EXPECT_EQ(ids.size(), kSigVerifyNumRsaKeys);
}

}  // namespace
}  // namespace rsa_verify_unittest
