#include "robot.hpp"
#include "amv.h"
#include "debug.hpp"
#include "util.hpp"
#include "hsvconverter.hpp"
#include <set>
#include <queue>

using namespace std;
using namespace cv;


vector<double> getSecondaryPatches(Patch* patch, amv_team_info* team, Image* debug) {
    std::set<Point, bool(*)(Point, Point)> visited(comparePoint);
    Point p = patch->getCenter();
    double angle = patch->getAngle();

    double front_x = cos(angle);
    double front_y = sin(angle);
    angle-=M_PI/2;
    double side_x = cos(angle);
    double side_y = sin(angle);

    double px_per_cm = patch->type->config->px_per_cm;
    double len_max = 3.28*px_per_cm;
    double height_min = 2.34*px_per_cm;
    double height_max = 3.75*px_per_cm;

    PatchFinder* pf = patch->type->map;
    amv_config* config = patch->type->config;
    PatchMoments colors[AMV_MAX_SECONDARY_COLORS];
    for (double i=-len_max; i <= len_max; i+=0.5)
        for (double j=height_min; j <= height_max;  j+=0.5) {
            Point q(p.x + front_x*i + side_x*j, p.y + front_y*i + side_y*j);
            if (q.x<0 || q.y <0 || q.x >= pf->img.cols || q.y >= pf->img.rows)
                continue;
            if (pf->area_map.get(q.x, q.y))
                continue;
            if (j + i >= height_min + len_max || j - i >= height_min + len_max)
                continue;
            if (visited.find(q) != visited.end())
                continue;
            visited.insert(q);

            int in_index = -1;
            Vec3b hsv = pf->img_hsv(q);
            if (debug)
                (*debug)(q)+=(*debug)(q);
            int hue = hsv[0];
            for (int k=0; k<AMV_MAX_SECONDARY_COLORS; k++) {
                amv_color_info c = team->secondary_colors[k];
                if (c.hue_min == c.hue_max) {
                    if (hsv[2] < config->black_cutoff/2) {
                        in_index = k;
                    }
                } else {
                    if (in_hue(&c, hue))
                        in_index = k;
                }
            }
            if (in_index == -1)
                continue;

            colors[in_index].add(q);
            if (debug) {
                amv_color_info c = team->secondary_colors[in_index];
                Vec3b paint = getMeanColor(c);
                drawPoint(*debug, q, paint);
            }

        }
    vector<double> scores(team->team_size, -1.0);
    for (int i=0; i<team->team_size; i++) {
        PatchMoments front = colors[team->robot_info[i].front_color];
        PatchMoments back = colors[team->robot_info[i].back_color];
        double radiusFront = sqrt((double)front.getCount());
        double radiusBack = sqrt((double)back.getCount());
        double distanceX = front.getMean().x - back.getMean().x;
        double distanceY = front.getMean().y - back.getMean().y;
        double distance = distanceX*front_x + distanceY*front_y;
        double score = pow(radiusFront + radiusBack, 8) + distance;
        if (distance < 0) {
            score -=1;
        }
        if (radiusBack == 0 || radiusFront == 0)
            score = -1e9;
        scores[i] = score;
    }
    return scores;
}

vector<Robot> getTeam(PatchType* patchType, amv_team_info* teami) {
    vector<Robot> out;
    std::priority_queue<std::pair<double, Patch*> > team;

    for (unsigned int i=0; i<patchType->patches.size(); i++) {
        Patch* patch = patchType->patches[i];
        if (patch->getCount() <= 1)
            continue;
        team.push(std::make_pair(-patch->getRobotCertainty(), patch));

        if (team.size() > teami->team_size)
            team.pop();
    }

    while (!team.empty()) {
        Robot r;
        Patch* patch = team.top().second;
        team.pop();

        if (teami->home_team) {
            r.pos = patch->getRobotCenter();
            r.angle = patch->getRobotAngle();
        } else {
            r.pos = patch->getCenter();
            r.angle = 0;
        }
        r.certainty = patch->getRobotCertainty();
        r.teamPatch = patch;
        r.identityCertainities = getSecondaryPatches(patch, teami);
        r.identity = -1;
        patch->isRobot = true;
        out.push_back(r);
    }
    if (teami->home_team) {
        vector<pair<double, pair<int, int> > > edges;
        for (int i=0; i<out.size(); i++) {
            for (int j=0; j<out[i].identityCertainities.size(); j++) {
                double score = out[i].identityCertainities[j];
                edges.push_back(make_pair(-score, make_pair(i, j)));
            }
        }
        sort(edges.begin(), edges.end());

        vector<bool> identityReservations(teami->team_size, false);
        for (int i=0; i<edges.size(); i++) {
            int robot = edges[i].second.first;
            int identity = edges[i].second.second;
            //cout<<"edge" << robot << "[" << out[robot].pos.x << ", " << out[robot].pos.y <<"] -> "<<identity<<" w "<<-edges[i].first<<endl;
            if (out[robot].identity != -1)
                continue;
            if (identityReservations[identity])
                continue;
            out[robot].identity = identity;
            identityReservations[identity] = true;
        }
    }
    return out;
}

static bool compare_robot_identity(const Robot& a, const Robot& b) {
    return a.identity < b.identity;
}

void fillTeam(vector<Robot> team, amv_team_data* data) {
    sort(team.begin(), team.end(), compare_robot_identity);

    data->team_len = 0;
    amv_robot_data* robot = data->team;
    for (unsigned int i = 0; i<team.size(); i++) {
        Robot patch = team[i];
        robot->position.x = patch.pos.x;
        robot->position.y = patch.pos.y;
        robot->angle = patch.angle;
        robot->certainty = patch.certainty;
        robot->identity = patch.identity;
        robot++;
        data->team_len++;
    }
}

void fillBall(Patch* ball, amv_vision_data* data) {
    if (!ball) {
        data->ball_pos.x = data->ball_pos.y = -1;
        return;
    }
    data->ball_pos.x = ball->getCenter().x;
    data->ball_pos.y = ball->getCenter().y;
}

void transformTeam(amv_team_data* robots, amv_transform_info transform) {
    for (int i=0; i<robots->team_len; i++) {
        transformPosition(&robots->team[i].position, transform);
    }
}
