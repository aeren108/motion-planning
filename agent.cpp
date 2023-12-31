#include "agent.h"
#include <chrono> 
//TODO integrate with ros

std::vector<agent*> agent::agents;
std::vector<obstacle> obs;

agent::agent() : pos(_pos), vel(_vel) {
    agents.push_back(this);
    obs.push_back(obstacle( 1,  1, 0.1));
    obs.push_back(obstacle(0.5, 1, 0.1));
    obs.push_back(obstacle( 0,  1, 0.1));
    obs.push_back(obstacle(-1, -1, 0.1));
}

agent::agent(vector3f pos) : pos(_pos), vel(_vel) {
    _pos = pos;
    agents.push_back(this);
    obs.push_back(obstacle( 1,  1, 0.1));
    obs.push_back(obstacle(0.5, 1, 0.1));
    obs.push_back(obstacle( 0,  1, 0.1));
    obs.push_back(obstacle(-1, -1, 0.1));
}

void agent::add_objective(vector2f p) {
    objectives.push(p);
}

void agent::clear_objectives() {
    for (int i = 0; i < objectives.size(); objectives.pop(), ++i);
}

vector2f agent::cur_objective() {
    return objectives.front();
}

void agent::start(bool apf) {
    thread = std::thread(apf ? &agent::_start_apf : &agent::_start_prm, this);
}

void agent::_start_apf() {
    active = true;
    while(objectives.size() != 1) objectives.pop();
    vector2f objective = objectives.front();
    apf(vector3f(objective.x, objective.y, 0));

    active = false;
}

void agent::_start_prm() {
    active = true;
    while (!objectives.empty()) {
        go_to(objectives.front());
        objectives.pop();
    }

    active = false;
}

void agent::stop() {
    active = false;
    while (!objectives.empty()) objectives.pop();
    thread.join();
}

void agent::pause() {

}

void agent::wait() {
    if (!completed()) return;
    thread.join();
}

void agent::wait_all() {
    for (agent* a : agents) a->wait();
}

bool agent::completed() const {
    return !active && objectives.size() == 0 ? true : false;
}

bool agent::completed_all() {
    for (agent* a : agents) if (!a->completed()) return false;
    return true;
}

//TODO handle collisions
void agent::go_to(vector3f p) {
    vector3f err, prev_err;
    vector3f integral, derivative;
    vector3f input;
    float dt = 0.01;

    while (active && !reached(p)) { 
        vector3f err = p - pos;
        integral = integral + (err * dt);
        derivative = (err - prev_err) / dt;

        input = (err*KP) + (integral*KI) + (derivative*KD);
        prev_err = err;
        
        _pos = _pos + input*dt;
        _vel = input;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        
        //publish input as velocity
        
    }

    std::cout << pos.x << ", " << pos.y << std::endl;
}

void agent::go_to(vector2f p) {
    go_to(vector3f(p.x, p.y, pos.z));
}

void agent::apf(vector3f p) {

    vector3f repl, attr, zero;

    while (active && !reached(p)) {
        repl.x = 0; repl.y = 0; attr.x = 0; attr.y = 0;
        for (agent* a : agents) {
            if (a == this) continue;
            if (dist3(pos, a->pos) < 0.75) {
                vector3f dir = pos - a->pos; dir = dir / dist3(dir, zero);
                repl = repl + (dir * (0.05 * 1/pow(dist3(pos, a->pos),3)));
            }
        }
        obstacle min = obs[0]; vector3f minv(min.x, min.y, 0);
        for (obstacle o : obs) {
            vector3f opos(o.x, o.y, 0);
            if (dist3(opos, pos) < dist3(pos, minv)) {
                min = o;
                minv = vector3f(min.x, min.y, 0);
            }
            
        }

        float d = dist3(pos, minv) - min.radius;
        if (d < 0.5) {
            vector3f dir = pos - minv; dir = dir / dist3(dir, zero);
            repl = repl + (dir * (0.001 * 1/pow(d,3)));
        }
        

        attr = ((p - pos) * 0.75) / dist3(p, pos);

        _vel = repl + attr;
        if (dist3(vel, zero) > 3) _vel = (_vel / dist3(vel, zero))*3;
        _pos = _pos + (_vel * 0.01);

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

/*/
void agent::go_to() {
    vector3f p = vector3f(objectives.front().x, objectives.front().y, 0);
    vector3f n = vector3f(objectives.front().x, objectives.front().y, 0);
    vector3f err, prev_err;
    vector3f integral, derivative;
    vector3f input;
    float dt = 0.01;

    while (active && !reached(p)) { 
        vector3f err = p - pos;
        integral = integral + (err * dt);
        derivative = (err - prev_err) / dt;

        input = (err*KP) + (integral*KI) + (derivative*KD);
        prev_err = err;
        
        _pos = _pos + input*dt;
        _vel = input;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        
        //publish input as velocity
        if (dist3(p, n) > 0.05) {
            vector3f zero, dir = n - p;
            dir = dir / dist3(dir, zero);
            dir = dir * 0.01;

            p = p + dir;
        } else if (!objectives.empty()) { objectives.pop(); n = vector3f(objectives.front().x, objectives.front().y, 0);}
    }

    std::cout << pos.x << ", " << pos.y << std::endl;
}*/

bool agent::reached(vector3f p) const {
    return dist3(pos, p) < ERROR ? true : false;
}

void agent::fetch_pos() {
    //retrieve pos info from related ros topic
}

agent::~agent() {
    //remove from agents list
    thread.join();
}

