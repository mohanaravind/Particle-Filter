/*
 * particle_filter.cpp
 *
 *  Created on: Dec 12, 2016
 *      Author: Tiffany Huang
 */

#include <random>
#include <algorithm>
#include <iostream>
#include <numeric>
#include <math.h> 
#include <iostream>
#include <sstream>
#include <string>
#include <iterator>

#include "particle_filter.h"

using namespace std;

void ParticleFilter::init(double x, double y, double theta, double std[]) {
  // TODO: Set the number of particles. Initialize all particles to first position (based on estimates of 
  //   x, y, theta and their uncertainties from GPS) and all weights to 1. 
  // Add random Gaussian noise to each particle.
  // NOTE: Consult particle_filter.h for more information about this method (and others in this file).

  // set the number of particles
  num_particles = 5;

  // loop through each particle
  for (int idx = 0; idx < num_particles; idx++) {
    // initialize all particles to first position (based on GPS estimates)
    double x_p = get_normal_value(x, std[0]);
    double y_p = get_normal_value(y, std[1]);    
    double theta_p = get_normal_value(theta, std[2]);
    double weight = 1.0;

    // create a particle and push it to the list
    Particle particle = { idx, x_p, y_p, theta_p, weight };
    particles.push_back(particle);
  }
      
  // Set it as initialized
  is_initialized = true;
}

void ParticleFilter::prediction(double delta_t, double std_pos[], double velocity, double yaw_rate) {
	// TODO: Add measurements to each particle and add random Gaussian noise.
	// NOTE: When adding noise you may find std::normal_distribution and std::default_random_engine useful.
	//  http://en.cppreference.com/w/cpp/numeric/random/normal_distribution
	//  http://www.cplusplus.com/reference/random/default_random_engine/

  // loop through each particle
  for (int idx = 0; idx < num_particles; idx++) {
    // get the particle info.
    Particle particle = particles[idx];
    double x = particle.x;
    double y = particle.y;
    double theta = particle.theta;    

    // compute the new values assuming bicycle motion
    x += (velocity / yaw_rate) * (+sin(theta + yaw_rate * delta_t) - sin(theta));
    y += (velocity / yaw_rate) * (-cos(theta + yaw_rate * delta_t) + cos(theta));
    theta += yaw_rate * delta_t;

    // Add random Gaussian noise and store update the particle
    particle.x = get_normal_value(x, std_pos[0]);
    particle.y = get_normal_value(y, std_pos[1]);
    particle.theta = get_normal_value(theta, std_pos[2]);    
  }
}

void ParticleFilter::dataAssociation(std::vector<LandmarkObs> predicted, std::vector<LandmarkObs>& observations) {
	// TODO: Find the predicted measurement that is closest to each observed measurement and assign the 
	//   observed measurement to this particular landmark.
	// NOTE: this method will NOT be called by the grading code. But you will probably find it useful to 
	//   implement this method and use it as a helper during the updateWeights phase.
  
  // loop through all the predicted landmarks
  vector<LandmarkObs>::iterator p_it;
  for (p_it = predicted.begin(); p_it != predicted.end(); ++p_it) {
    // predicted landmark
    LandmarkObs prd = *p_it;

    // initialize
    double shortest_dist = numeric_limits<float>::max();    

    // loop through all the observed landmarks
    vector<LandmarkObs>::iterator o_it;
    for (o_it = observations.begin(); o_it != observations.end(); ++o_it) {
      // observed landmark
      LandmarkObs obs = *o_it;

      // compute the euclidean distance
      double euclidean_dist = dist(prd.x, prd.y, obs.x, obs.y);      

      // if shorter distance or its the first iteration
      if (euclidean_dist < shortest_dist || shortest_dist == NULL) {
        shortest_dist = euclidean_dist;

        // assign the observed measurement to this landmark
        prd.id = obs.id;
        break;
      }
    }
  }    

}

void ParticleFilter::updateWeights(double sensor_range, double std_landmark[], 
		const std::vector<LandmarkObs> &observations, const Map &map_landmarks) {
	// TODO: Update the weights of each particle using a mult-variate Gaussian distribution. You can read
	//   more about this distribution here: https://en.wikipedia.org/wiki/Multivariate_normal_distribution
	// NOTE: The observations are given in the VEHICLE'S coordinate system. Your particles are located
	//   according to the MAP'S coordinate system. You will need to transform between the two systems.
	//   Keep in mind that this transformation requires both rotation AND translation (but no scaling).
	//   The following is a good resource for the theory:
	//   https://www.willamette.edu/~gorr/classes/GeneralGraphics/Transforms/transforms2d.htm
	//   and the following is a good resource for the actual equation to implement (look at equation 
	//   3.33
	//   http://planning.cs.uiuc.edu/node99.html


}

void ParticleFilter::resample() {
	// TODO: Resample particles with replacement with probability proportional to their weight. 
	// NOTE: You may find std::discrete_distribution helpful here.
	//   http://en.cppreference.com/w/cpp/numeric/random/discrete_distribution

}

Particle ParticleFilter::SetAssociations(Particle& particle, const std::vector<int>& associations, 
                                     const std::vector<double>& sense_x, const std::vector<double>& sense_y)
{
    //particle: the particle to assign each listed association, and association's (x,y) world coordinates mapping to
    // associations: The landmark id that goes along with each listed association
    // sense_x: the associations x mapping already converted to world coordinates
    // sense_y: the associations y mapping already converted to world coordinates

    particle.associations= associations;
    particle.sense_x = sense_x;
    particle.sense_y = sense_y;

    return particle;
}

string ParticleFilter::getAssociations(Particle best)
{
	vector<int> v = best.associations;
	stringstream ss;
    copy( v.begin(), v.end(), ostream_iterator<int>(ss, " "));
    string s = ss.str();
    s = s.substr(0, s.length()-1);  // get rid of the trailing space
    return s;
}
string ParticleFilter::getSenseX(Particle best)
{
	vector<double> v = best.sense_x;
	stringstream ss;
    copy( v.begin(), v.end(), ostream_iterator<float>(ss, " "));
    string s = ss.str();
    s = s.substr(0, s.length()-1);  // get rid of the trailing space
    return s;
}
string ParticleFilter::getSenseY(Particle best)
{
	vector<double> v = best.sense_y;
	stringstream ss;
    copy( v.begin(), v.end(), ostream_iterator<float>(ss, " "));
    string s = ss.str();
    s = s.substr(0, s.length()-1);  // get rid of the trailing space
    return s;
}
