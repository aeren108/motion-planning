#ifndef AGENT_H
#define AGENT_H

#include <iostream>
#include <queue>
#include <vector>
#include <cmath>
#include <thread>

#define dist2(v1, v2) sqrt(pow(v1.x - v2.x, 2) + pow(v1.y - v2.y, 2))
#define dist3(v1, v2) sqrt(pow(v1.x - v2.x, 2) + pow(v1.y - v2.y, 2) + pow(v1.z - v2.z, 2))
 
#define KP 1.875
#define KI 0.2
#define KD 0.24
#define ERROR 0.005
#define AGENT_RADIUS 0.05

struct vector2f {
    float x, y;
    vector2f() : x(0), y(0) {}
    vector2f(float x, float y) : x(x), y(y) {}

    vector2f operator+(vector2f const& rhs) const { 
        return vector2f(x+rhs.x, y+rhs.y);
    }

    vector2f operator-(vector2f const& rhs) const { 
        return vector2f(x-rhs.x, y-rhs.y);
    }

    vector2f operator*(float c) const {
        return vector2f(x*c, y*c);
    }

    vector2f operator/(float c) const {
        return vector2f(x/c, y/c);
    }
};

struct vector3f {
    float x, y, z;
    vector3f() : x(0), y(0), z(0) {}
    vector3f(float x, float y, float z) : x(x), y(y), z(z) {}

    vector3f operator+(vector3f const& rhs) const { 
        return vector3f(x+rhs.x, y+rhs.y, z+rhs.z);
    }

    vector3f operator-(vector3f const& rhs) const { 
        return vector3f(x-rhs.x, y-rhs.y, z-rhs.z);
    }

    vector3f operator*(float c) const {
        return  vector3f(x*c, y*c, z*c);
    }

    vector3f operator/(float c) const {
        return vector3f(x/c, y/c, z/c);
    }
};

class agent {
    public:
        agent();
        agent(vector3f pos);

        void add_objective(vector2f p);
        void clear_objectives();
        vector2f cur_objective();
        void start(bool apf);
        void stop();
        void pause();
        bool completed() const;
        void wait();
        
        static bool completed_all();
        static void wait_all();
        
        ~agent();
        
        static std::vector<agent*> agents;
        const vector3f& pos;
        const vector3f& vel;
        int priority;
        
    private:
        void _start_apf();
        void _start_prm();
        void go_to(vector2f p);
        void go_to(vector3f p);
        void apf(vector3f);
        bool reached(vector3f p) const;
        void fetch_pos();
        
        bool active;
        std::queue<vector2f> objectives;
        std::thread thread;
        vector3f _pos, _vel;
};

struct obstacle {
    float x, y;
    float radius;

    obstacle(float x, float y, float r) : x(x), y(y), radius(r) {}
};

#endif