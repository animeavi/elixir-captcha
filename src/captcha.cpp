/*
 #
 #  File        : captcha.cpp
 #                ( C++ source file )
 #
 #  Description : Captcha images generator.
 #                This file is a part of the CImg Library project.
 #                ( http://cimg.eu )
 #
 #  Copyright   : David Tschumperlé
 #                ( http://tschumperle.users.greyc.fr/ )
 #
 #  License     : CeCILL v2.0
 #                ( http://www.cecill.info/licences/Licence_CeCILL_V2-en.html )
 #
 #  This software is governed by the CeCILL  license under French law and
 #  abiding by the rules of distribution of free software.  You can  use,
 #  modify and/ or redistribute the software under the terms of the CeCILL
 #  license as circulated by CEA, CNRS and INRIA at the following URL
 #  "http://www.cecill.info".
 #
 #  As a counterpart to the access to the source code and  rights to copy,
 #  modify and redistribute granted by the license, users are provided only
 #  with a limited warranty  and the software's author,  the holder of the
 #  economic rights,  and the successive licensors  have only  limited
 #  liability.
 #
 #  In this respect, the user's attention is drawn to the risks associated
 #  with loading,  using,  modifying and/or developing or reproducing the
 #  software by the user in light of its specific status of free software,
 #  that may mean  that it is complicated to manipulate,  and  that  also
 #  therefore means  that it is reserved for developers  and  experienced
 #  professionals having in-depth computer knowledge. Users are therefore
 #  encouraged to load and test the software's suitability as regards their
 #  requirements in conditions enabling the security of their systems and/or
 #  data to be ensured and,  more generally, to use and operate it in the
 #  same conditions as regards security.
 #
 #  The fact that you are presently reading this means that you have had
 #  knowledge of the CeCILL license and that you accept its terms.
 #
*/

#include <algorithm>
#include <array>
#include <cstring>
#include <functional>
#include <iostream>
#include <random>
#include <string>

#ifndef cimg_debug
#define cimg_debug 1
#endif
#include "CImg.h"
using namespace cimg_library;
#undef min
#undef max

// https://stackoverflow.com/a/444614
template <typename T = std::mt19937>
auto random_generator() -> T {
    auto constexpr seed_bytes = sizeof(typename T::result_type) * T::state_size;
    auto constexpr seed_len = seed_bytes / sizeof(std::seed_seq::result_type);
    auto seed = std::array<std::seed_seq::result_type, seed_len>();
    auto dev = std::random_device();
    std::generate_n(begin(seed), seed_len, std::ref(dev));
    auto seed_seq = std::seed_seq(begin(seed), end(seed));
    return T{seed_seq};
}

auto generate_random_captcha(std::size_t len) -> std::string {
    static constexpr auto chars =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    thread_local auto rng = random_generator<>();
    auto dist = std::uniform_int_distribution{{}, std::strlen(chars) - 1};
    auto result = std::string(len, '\0');
    std::generate_n(begin(result), len, [&]() { return chars[dist(rng)]; });
    return result;
}

// Main procedure
//----------------
int main(int argc,char **argv) {

  // Read command line parameters
  //------------------------------
  cimg_usage("Simple captcha generator.");
  const char *file_o       = cimg_option("-o",(const char*)0,"Output image file");
  const bool add_border    = cimg_option("-b",true,"Add border to captcha image");
  const int length         = cimg_option("-l",5,"Set the length of the captcha");

  // Generate captcha text
  //------------------------------------
  const char *const captcha_text = generate_random_captcha(length).c_str();

  // Create captcha image
  //----------------------

  // Write colored and distorted text
  CImg<unsigned char> captcha(256,64,1,3,0), color(3);
  char letter[2] = { 0 };
  for (unsigned int k = 0; k<6; ++k) {
    CImg<unsigned char> tmp;
    *letter = captcha_text[k];
    if (*letter) {
      cimg_forX(color,i) color[i] = (unsigned char)(128 + (std::rand()%127));
      tmp.draw_text((int)(2 + 8*cimg::rand()),
                    (int)(12*cimg::rand()),
                    letter,color.data(),0,1,std::rand()%2?38:57).resize(-100,-100,1,3);
      const unsigned int dir = std::rand()%4, wph = tmp.width() + tmp.height();
      cimg_forXYC(tmp,x,y,v) {
        const int val = dir==0?x + y:(dir==1?x + tmp.height() - y:(dir==2?y + tmp.width() - x:
                                                                   tmp.width() - x + tmp.height() - y));
        tmp(x,y,v) = (unsigned char)std::max(0.0f,std::min(255.0f,1.5f*tmp(x,y,v)*val/wph));
      }
      if (std::rand()%2) tmp = (tmp.get_dilate(3)-=tmp);
      tmp.blur((float)cimg::rand()*0.8f).normalize(0,255);
      const float sin_offset = (float)cimg::rand(-1,1)*3, sin_freq = (float)cimg::rand(-1,1)/7;
      cimg_forYC(captcha,y,v) captcha.get_shared_row(y,0,v).shift((int)(4*std::cos(y*sin_freq + sin_offset)));
      captcha.draw_image(6 + 40*k,tmp);
    }
  }

  // Add geometric and random noise
  CImg<unsigned char> copy = (+captcha).fill(0);
  for (unsigned int l = 0; l<3; ++l) {
    if (l) copy.blur(0.5f).normalize(0,148);
    for (unsigned int k = 0; k<10; ++k) {
      cimg_forX(color,i) color[i] = (unsigned char)(128 + cimg::rand()*127);
      if (cimg::rand()<0.5f) copy.draw_circle((int)(cimg::rand()*captcha.width()),
                                              (int)(cimg::rand()*captcha.height()),
                                              (int)(cimg::rand()*30),
                                              color.data(),0.6f,~0U);
      else copy.draw_line((int)(cimg::rand()*captcha.width()),
                          (int)(cimg::rand()*captcha.height()),
                          (int)(cimg::rand()*captcha.width()),
                          (int)(cimg::rand()*captcha.height()),
                          color.data(),0.6f);
    }
  }
  captcha|=copy;
  captcha.noise(10,2);

  if (add_border)
    captcha.draw_rectangle(0,0,captcha.width() - 1,captcha.height() - 1,
                           CImg<unsigned char>::vector(255,255,255).data(),1.0f,~0U);
  captcha = (+captcha).fill(255) - captcha;

  // Write output image and captcha text
  //-------------------------------------
  if (file_o) {
    std::cout << captcha_text <<std::endl;
    captcha.save(file_o);
  } else {
    std::cout << captcha_text;
    FILE* fp = stdout;
    captcha.save_png(fp);
  }

  return 0;
}
