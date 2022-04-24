#ifndef _DEMO_HPP_
#define _DEMO_HPP_

#include <iostream>
#include <ctime>
#include <cmath>
#include <array>
#include <vector>
#include <cassert>

#include "Commons.hpp"
#include "Video.h"
#include "Fly.hpp"
#include "EmptyAllocator.hpp"

namespace g80 {

    class Demo : public Video {

    public: 
        Demo (const Dim N, const Dim TAIL = 5) : 
            N_(N),
            BACKGROUND_N_(N_ * 0.95), 
            FACE_N_((N_ - BACKGROUND_N_) * 0.75),
            SMILE_N_(((N_ - BACKGROUND_N_) - FACE_N_) * 0.75),
            EYE1_N_(((N_ - BACKGROUND_N_) - FACE_N_ - SMILE_N_) * 0.5),
            EYE2_N_(((N_ - BACKGROUND_N_) - FACE_N_ - SMILE_N_ - EYE1_N_) * 0.5),
            TAIL_(TAIL) {}

        auto create_window(const VideoConfig &video_config) -> bool;
        auto preprocess_states() -> bool;
        auto update_states() -> bool;
        auto capture_events() -> bool;

    private:
        const Dim N_;
        const Dim BACKGROUND_N_ ;
        const Dim FACE_N_;
        const Dim SMILE_N_;
        const Dim EYE1_N_;
        const Dim EYE2_N_;
        const Dim TAIL_;

        Dim FACE_LOCATION_RADIUS_;
        Dim SMILE_LOCATION_RADIUS_;
        Dim EYE_OFFSET_;
        Dim FLY_RADIUS_;
        Dim MAX_FLY_INIT_ANGLE{20};

        Flies flies_;
        std::array<float, 360> cosf_;
        std::array<float, 360> sinf_;
        // std::vector<std::array<Dim, 360>> radius_cos_;
        // std::vector<std::array<Dim, 360>> radius_sin_;

        inline auto rnd() -> Dim32 {
            static std::time_t now = time(&now);
            static Dim32 seed = now;
            static Dim32 a = 1103515245;
            static Dim32 c = 12345;
            static Dim32 m = 2147483647;
            static RandNum rand = (seed * a + c) % m;
            return rand = (rand * a + c) % m; 
        }
        auto init_sincos_table() -> bool;
        auto init_reserved_flies() -> bool;
        auto init_background() -> bool;
        auto init_face() -> bool;
        auto init_smile() -> bool;
        auto init_eyes() -> bool;
        auto init_fly_tail() -> bool;
    };

    auto Demo::create_window(const VideoConfig &video_config) -> bool {
        if (!Video::create_window(video_config)) return false;
        FACE_LOCATION_RADIUS_ = {static_cast<Dim>(surface_->h / 2 * 0.70)},
        SMILE_LOCATION_RADIUS_ = {static_cast<Dim>(surface_->h / 2 * 0.70 * 0.50)},
        EYE_OFFSET_ = {static_cast<Dim>(SMILE_LOCATION_RADIUS_ * 0.25)},
        FLY_RADIUS_ = {static_cast<Dim>(FACE_LOCATION_RADIUS_ * 0.11)};
        return true;
    }

    auto Demo::init_sincos_table() -> bool {
        
        /*
         * Do sinf(angle) or cosf(angle)
         * instead of sinf(i * M_PI / 180) or cosf(i * M_PI / 180) 
         * to prevent back and forth int - float - int conversion.
         * This method also has lesser operations 
         *
         */

        float angle = 0.0f;
        float angle_inc = M_PI * 2.0f / 360.0f;
        for (AngleIx i = 0; i < 360; ++i) {            
            cosf_[i] = cosf(angle);     
            sinf_[i] = sinf(angle);     
            angle += angle_inc;
        }

        // Slower probably due to cache misses
        // 
        // radius_cos_.reserve(FLY_RADIUS_ + 1);
        // radius_sin_.reserve(FLY_RADIUS_ + 1);        
        // for (Dim i = 0; i <= FLY_RADIUS_; ++i) {
        //     radius_cos_.emplace_back(std::array<Dim, 360>());
        //     radius_sin_.emplace_back(std::array<Dim, 360>());
        //     for (Dim j = 0; j < 360; ++j) {
        //         radius_cos_[i][j] = i * cosf_[j];
        //         radius_sin_[i][j] = i * sinf_[j];
        //     }
        // }
        
        return true;
    }

    auto Demo::init_reserved_flies() -> bool {
        flies_.reserve(N_);
        return true;
    }

    auto Demo::init_background() -> bool {
        for (Dim i = 0; i < BACKGROUND_N_; ++i) {
            flies_.emplace_back(Fly({
                rnd() % surface_->w,
                rnd() % surface_->h,
                static_cast<Color>(SDL_MapRGBA(surface_->format, 0, 0, 100 + rnd() % 156, 255)),
                static_cast<Dim16>(1 + rnd() % MAX_FLY_INIT_ANGLE),
                static_cast<Dim16>(1 + rnd() % MAX_FLY_INIT_ANGLE),
                static_cast<Dim16>(1 + rnd() % FLY_RADIUS_),
                static_cast<Dim16>(1 + rnd() % FLY_RADIUS_)}));
        }        
        return true;
    }

    auto Demo::init_face() -> bool {
        float step_on = (FACE_N_ / 360.0f);
        for (Dim i = 0; i < FACE_N_; ++i) {
            flies_.emplace_back(Fly({
                static_cast<Dim32>(surface_->w / 2 + cosf_[i / step_on] * (FACE_LOCATION_RADIUS_ + rand() % FLY_RADIUS_)),
                static_cast<Dim32>(surface_->h / 2 + sinf_[i / step_on] * (FACE_LOCATION_RADIUS_ + rand() % FLY_RADIUS_)),
                static_cast<Color>(SDL_MapRGBA(surface_->format, 0, 50 + rnd() % 206, 0, 255)),
                static_cast<Dim16>(1 + rnd() % MAX_FLY_INIT_ANGLE),
                static_cast<Dim16>(1 + rnd() % MAX_FLY_INIT_ANGLE),
                static_cast<Dim16>(1 + rnd() % FLY_RADIUS_),
                static_cast<Dim16>(1 + rnd() % FLY_RADIUS_)}));
        }
        return true;
    }

    auto Demo::init_smile() -> bool {
        float step_on = (SMILE_N_ / 180.0f);
        for (Dim i = 0; i < SMILE_N_; ++i) {
            flies_.emplace_back(Fly({
                static_cast<Dim32>(surface_->w / 2 + cosf_[i/step_on] * (SMILE_LOCATION_RADIUS_ + rand() % FLY_RADIUS_)),
                static_cast<Dim32>(surface_->h / 2 + sinf_[i/step_on] * (SMILE_LOCATION_RADIUS_ + rand() % FLY_RADIUS_)),
                static_cast<Color>(SDL_MapRGBA(surface_->format, 50 + rnd() % 206, 0, 0, 255)),
                static_cast<Dim16>(1 + rnd() % MAX_FLY_INIT_ANGLE),
                static_cast<Dim16>(1 + rnd() % MAX_FLY_INIT_ANGLE),
                static_cast<Dim16>(1 + rnd() % FLY_RADIUS_),
                static_cast<Dim16>(1 + rnd() % FLY_RADIUS_)}));
        }
        return true;
    }

    auto Demo::init_eyes() -> bool {
        for (Dim i = 0; i < EYE1_N_; ++i) {
            flies_.emplace_back(Fly({
                static_cast<Dim32>(surface_->w / 2 - (EYE_OFFSET_ * 2) + cosf_[0] * (SMILE_LOCATION_RADIUS_ + rand() % FLY_RADIUS_)),
                static_cast<Dim32>(surface_->h / 2 - (EYE_OFFSET_ * 3) + sinf_[0] * (SMILE_LOCATION_RADIUS_ + rand() % FLY_RADIUS_)),
                static_cast<Color>(SDL_MapRGBA(surface_->format, 50 + rnd() % 206, 50 + rnd() % 206, 50 + rnd() % 206, 255)),
                static_cast<Dim16>(1 + rnd() % MAX_FLY_INIT_ANGLE),
                static_cast<Dim16>(1 + rnd() % MAX_FLY_INIT_ANGLE),
                static_cast<Dim16>(1 + rnd() % FLY_RADIUS_),
                static_cast<Dim16>(1 + rnd() % FLY_RADIUS_)}));
        }

        for (Dim i = 0; i < EYE2_N_; ++i) {
            flies_.emplace_back(Fly({
                static_cast<Dim32>(surface_->w / 2 + (EYE_OFFSET_ * 2) + cosf_[179] * (SMILE_LOCATION_RADIUS_ + rand() % FLY_RADIUS_)),
                static_cast<Dim32>(surface_->h / 2 - (EYE_OFFSET_ * 3) + sinf_[179] * (SMILE_LOCATION_RADIUS_ + rand() % FLY_RADIUS_)),
                static_cast<Color>(SDL_MapRGBA(surface_->format, 50 + rnd() % 206, 50 + rnd() % 206, 50 + rnd() % 206, 255)),
                static_cast<Dim16>(1 + rnd() % MAX_FLY_INIT_ANGLE),
                static_cast<Dim16>(1 + rnd() % MAX_FLY_INIT_ANGLE),
                static_cast<Dim16>(1 + rnd() % FLY_RADIUS_),
                static_cast<Dim16>(1 + rnd() % FLY_RADIUS_)}));
        }
        return true;
    }

    auto Demo::init_fly_tail() -> bool {
        for (auto &fly : flies_) {
            Dim max_offset_x = (360 / fly.xan) - TAIL_;
            Dim max_offset_y = (360 / fly.yan) - TAIL_;
            fly.xa = (rnd() % max_offset_x * fly.xan) + TAIL_ * fly.xan;
            fly.ya = (rnd() % max_offset_y * fly.yan) + TAIL_ * fly.yan;
            fly.xta = fly.xa - TAIL_ * fly.xan;
            fly.yta = fly.ya - TAIL_ * fly.yan;        
        }
        return true;
    }

    auto Demo::preprocess_states() -> bool {
        if (!init_sincos_table() ||
            !init_reserved_flies() ||
            !init_background() ||
            !init_face() ||
            !init_smile() ||
            !init_eyes() ||
            !init_fly_tail()) 
            return false;
        
        return true;
    }

    auto Demo::update_states() -> bool {

        SDL_LockSurface(surface_);

        // Erase
        for (auto &fly : flies_) {
            
            // This method is slightly faster than the commented code
            // which could be attributable to cache misses
            //
            // fly.cx + fly.xr * cosf_[fly.xta]; vs. fly.cx + radius_cos_[fly.xr][fly.xta];
            // 

            Dim x = fly.cx + fly.xr * cosf_[fly.xta];
            Dim y = fly.cy + fly.yr * sinf_[fly.yta];
            // Dim x = fly.cx + radius_cos_[fly.xr][fly.xta];
            // Dim y = fly.cy + radius_cos_[fly.yr][fly.yta];
            fly.xta = (fly.xta + fly.xan) % 360;
            fly.yta = (fly.yta + fly.yan) % 360;
            set_pixel(x, y, 0);
        }

        // Update and Draw
        for (auto &fly : flies_) {
            
            // This method is slightly faster than the commented code
            // which could be attributable to cache misses
            //
            // fly.cx + fly.xr * cosf_[fly.xa]; vs fly.cx + radius_cos_[fly.xr][fly.xa];
            //

            Dim x = fly.cx + fly.xr * cosf_[fly.xa];
            Dim y = fly.cy + fly.yr * sinf_[fly.ya];
            // Dim x = fly.cx + radius_cos_[fly.xr][fly.xa];
            // Dim y = fly.cy + radius_cos_[fly.yr][fly.ya];
            fly.xa = (fly.xa + fly.xan) % 360;
            fly.ya = (fly.ya + fly.yan) % 360;
            set_pixel(x, y, fly.c);
        }
        
        SDL_UnlockSurface(surface_);
        return true;
    }

    auto Demo::capture_events() -> bool {
        SDL_Event e;
        while(SDL_PollEvent(&e)) {
            if(e.type == SDL_QUIT)
                is_running_ = false;

            else if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_ESCAPE)
                    is_running_ = false;
            }
        }
        return true;
    }
}

#endif 