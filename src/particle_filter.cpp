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
#include <map>

#include "particle_filter.h"

using namespace std;

// define the random generator
static random_device rd;
static mt19937 gen(rd());

void ParticleFilter::init(double x, double y, double theta, double std[]) {
  // TODO: Set the number of particles. Initialize all particles to first position (based on estimates of 
  //   x, y, theta and their uncertainties from GPS) and all weights to 1. 
  // Add random Gaussian noise to each particle.
  // NOTE: Consult particle_filter.h for more information about this method (and others in this file).

  // set the number of particles
  num_particles = 15;

  // noise distributions with a mean of 0
  normal_distribution<double> x_noise(0.0, std[0]);
  normal_distribution<double> y_noise(0.0, std[1]);
  normal_distribution<double> theta_noise(0.0, std[2]);

  // loop through each particle
  for (int idx = 0; idx < num_particles; idx++) {
    // initialize all particles to first position (based on GPS estimates)
    double x_p = x + x_noise(gen);
    double y_p = y + y_noise(gen);
    double theta_p = theta + theta_noise(gen);
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

  // noise distributions with a mean of 0
  normal_distribution<double> x_noise(0.0, std_pos[0]);
  normal_distribution<double> y_noise(0.0, std_pos[1]);
  normal_distribution<double> theta_noise(0.0, std_pos[2]);

  for (Particle &particle : particles) {  
    double x = particle.x;
    double y = particle.y;
    double theta = particle.theta;    

    // compute the new values assuming bicycle motion
    //if yaw_rate is close to zero
    if (fabs(yaw_rate) < 0.00001) {
      x += velocity * cos(theta) * delta_t;
      y += velocity * sin(theta) * delta_t;
    }
    else {      
      x += (velocity / yaw_rate) * (+sin(theta + yaw_rate * delta_t) - sin(theta));
      y += (velocity / yaw_rate) * (-cos(theta + yaw_rate * delta_t) + cos(theta));
      theta += yaw_rate * delta_t;
    }
        
    // Add random Gaussian noise and store update the particle
    particle.x = x + x_noise(gen);
    particle.y = y + y_noise(gen);
    particle.theta = theta + theta_noise(gen);    
  }
}

void ParticleFilter::dataAssociation(std::vector<LandmarkObs> predicted, std::vector<LandmarkObs>& observations) {
  // TODO: Find the predicted measurement that is closest to each observed measurement and assign the 
  //   observed measurement to this particular landmark.
  // NOTE: this method will NOT be called by the grading code. But you will probably find it useful to 
  //   implement this method and use it as a helper during the updateWeights phase.

  // loop through all the observed landmarks
  for (LandmarkObs& obs : observations) {
    // initialize
    double shortest_dist = numeric_limits<float>::max();

    // loop through all the landmarks in the map and find the closest to our observation
    for (LandmarkObs prd : predicted) {
      // compute the euclidean distance
      double euclidean_dist = dist(prd.x, prd.y, obs.x, obs.y);

      // if shorter distance or its the first iteration
      if (euclidean_dist < shortest_dist) {
        shortest_dist = euclidean_dist;

        // the obsrved measurement is what we have in the map
        obs.id = prd.id;
      }
    }
  }
}

void ParticleFilter::updateWeights(double sensor_range, double std_landmark[],
  std::vector<LandmarkObs> observations, Map map_landmarks) {
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

  // for each particle...
  for (Particle &p : particles) {
    double theta = p.theta;

    // transform the observations(measurements) from particle's perspective to MAP's perspective    
    vector<LandmarkObs> obs_marks;
    for (LandmarkObs c : observations) {
      double x = p.x + (cos(theta) * c.x) - (sin(theta) * c.y);
      double y = p.y + (sin(theta) * c.x) + (cos(theta) * c.y);

      // add it to the vector
      obs_marks.push_back({c.id, x, y});
    }

    // build the predicted landmarks from the MAP
    vector<LandmarkObs> map_marks;
    map<int, LandmarkObs> mapped_map;
    for (Map::single_landmark_s landmark : map_landmarks.landmark_list) {
      // get the distance of the landmark from the particle
      float distance = dist(p.x, p.y, landmark.x_f, landmark.y_f);

      // check if the landmark is within the range of the sensor
      if (distance <= sensor_range) {
        LandmarkObs map_landmark = { landmark.id_i, landmark.x_f, landmark.y_f };
        map_marks.push_back(map_landmark);
        mapped_map[landmark.id_i] = map_landmark;
      }
    }

    // perform dataAssociation for the predictions and transformed observations on current particle
    dataAssociation(map_marks, obs_marks);

    float prob = 1.0;
    // calculate the particle's weight using multi-variate Gaussian distribution
    for (LandmarkObs c : obs_marks) {
      // get the associated landmark from the map
      LandmarkObs landmark = mapped_map[c.id];

      float x = c.x;
      float y = c.y;
      float mu_x = landmark.x;
      float mu_y = landmark.y;
      float std_x = std_landmark[0];
      float std_y = std_landmark[1];

      float num = pow(x - mu_x, 2) / (2 * std_x * std_x) +
                  pow(y - mu_y, 2) / (2 * std_y * std_y);

      prob *= (1 / (2 * M_PI * std_x * std_y)) * exp(-num);
    }

    // update the weight of the particle
    p.weight = prob;

  }
}

void ParticleFilter::resample() {
  // TODO: Resample particles with replacement with probability proportional to their weight. 
  // NOTE: You may find std::discrete_distribution helpful here.
  //   http://en.cppreference.com/w/cpp/numeric/random/discrete_distribution

  double total_weights = 0.0;

  // build the weights vector
  weights.clear();
  for (Particle particle : particles) {
    weights.push_back(particle.weight);
    total_weights += particle.weight;
  }

  // if the total weights prob is close to zero then we don't resample
  if (fabs(total_weights) < 0.00001) {
    return;
  }

  // define a discrete distribution using the weights of particles
  discrete_distribution<int> dist(weights.begin(), weights.end());

  // do resampling from distribution
  int particles_count = particles.size();
  // to store resampled particles
  vector<Particle> resampled;
  for (int idx = 0; idx < particles_count; idx++) {
    // get the sample
    int sample_idx = dist(gen);
    resampled.push_back(particles[sample_idx]);
  }

  // update the particles vector with the resamled list
  particles = resampled;
}

Particle ParticleFilter::SetAssociations(Particle& particle, const std::vector<int>& associations,
  const std::vector<double>& sense_x, const std::vector<double>& sense_y)
{
  //particle: the particle to assign each listed association, and association's (x,y) world coordinates mapping to
  // associations: The landmark id that goes along with each listed association
  // sense_x: the associations x mapping already converted to world coordinates
  // sense_y: the associations y mapping already converted to world coordinates

  particle.associations = associations;
  particle.sense_x = sense_x;
  particle.sense_y = sense_y;

  return particle;
}

string ParticleFilter::getAssociations(Particle best)
{
  vector<int> v = best.associations;
  stringstream ss;
  copy(v.begin(), v.end(), ostream_iterator<int>(ss, " "));
  string s = ss.str();
  s = s.substr(0, s.length() - 1);  // get rid of the trailing space
  return s;
}
string ParticleFilter::getSenseX(Particle best)
{
  vector<double> v = best.sense_x;
  stringstream ss;
  copy(v.begin(), v.end(), ostream_iterator<float>(ss, " "));
  string s = ss.str();
  s = s.substr(0, s.length() - 1);  // get rid of the trailing space
  return s;
}
string ParticleFilter::getSenseY(Particle best)
{
  vector<double> v = best.sense_y;
  stringstream ss;
  copy(v.begin(), v.end(), ostream_iterator<float>(ss, " "));
  string s = ss.str();
  s = s.substr(0, s.length() - 1);  // get rid of the trailing space
  return s;
}
