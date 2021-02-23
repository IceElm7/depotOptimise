// Week 10 Intro to Programming Assignment Optimise GB Places
// Joseph Amoss (9974614) Year 2 Physics

#include <iostream>
#include <cmath>
#include <fstream>
#include <vector>
#include <string>

#define PI 3.14159

using namespace std;

// formula to calculate the Haversine Distance, with 2 inputs of Latitude and Longitude
double haversine_Distance(double Lat1, double Lat2, double Long1, double Long2){
    double RLat1 = (PI/180)*Lat1;
    double RLat2 = (PI/180)*Lat2;
    double RLong1 = (PI/180)*Long1;
    double RLong2 = (PI/180)*Long2;

    double dLat = RLat2 - RLat1;
    double dLong = RLong2 - RLong1;

    double a = pow(sin(dLat/2),2) + cos(RLat1)*cos(RLat2)*pow(sin(dLong/2),2);
    double c = 2*atan2(pow(a,0.5),pow((1-a),0.5));
    double D = 3958.75*c;
    return D;
}

// Structure of vectors to hold all the information of each place, can call any part of information required
// Vector pushed back onto Info is 'Data' (created below)
struct Info{
    vector <string> name;
    vector <string> type;
    vector <int> pop;
    vector <double> Lat;
    vector <double> Long;
};

// General random number generator to pick between any 2 values
double random_number (double lower, double upper, int n) {
    double r;
    r = lower + (rand() % (n + 1) * (1./n) * (upper - lower));
    return r;
}

// Formula to calculate the total distance from a trial point
// Adds up all the Haversine distances
// Note, the Haversine is weighted by the population of the location
double totalDistance(Info Data, double lat_try, double long_try) {
    double totalDist;
    for (int i = 0; i < Data.Long.size(); i++) {
        totalDist = totalDist + (haversine_Distance(lat_try, Data.Lat[i], long_try, Data.Long[i] ))/(Data.pop[i]);
    }
    return totalDist;
}

int main () {

    // open the file
    ifstream myFile("../GBPlaces.csv");

    //declare data that's being taken from csv and stored
    string place, type, population, latitude, longitude;

    // Declaring that Data will be stored in the structure Info
    struct Info Data;

    // name a string to call the first line
    string dummyLine;
    // pull out the first "dummy line" and does nothing with it
    getline(myFile, dummyLine);

    // if the file is open, and not at the end, splits up each line and names each variable
    if (myFile.is_open()) {
        while (!myFile.eof()) {
            getline(myFile, place, ',');
            getline(myFile, type, ',');
            getline(myFile, population, ',');
            getline(myFile, latitude, ',');
            getline(myFile, longitude);

            // Convert all the information from string to appropriate form
            int Population = stoi(population.c_str());
            double Latitude = stod(latitude.c_str());
            double Longitude = stod(longitude.c_str());

            //Push back elements onto data structure vector
            Data.name.push_back(place);
            Data.type.push_back(type);
            Data.pop.push_back(Population);
            Data.Lat.push_back(Latitude);
            Data.Long.push_back(Longitude);

        }
    myFile.close();

        // if unable to open, error message appears
    } else {
            cout << "Cannot open file";
            exit(1);
        }

    // declare variables to test where the minimum total distance is
    double lat_try, long_try, value, oldValue, newValue, dlat, dlong;
    // small step to hone in on location of minimum exactly
    double step = 0.001;
    // large initial minimum to compare values to, ensures all distances are considered
    double globalMin = 1e10;
    double gl_lat, gl_long;
    // seeds random number, different each time
    srand(time(NULL));

    // tests 100 points within range of GB places
    for( int k = 0; k<100;k++) {
        lat_try = random_number(50.36, 57.2, 100);
        long_try = random_number(-4.26, 1.3, 100);

        // value to be minimised is found by trialling the random values in the function
        value = totalDistance(Data, lat_try, long_try);
        // outputs where the code is, which trial values are being used
        cout << "Test " << k+1 << " Lat: " << lat_try << ". Long: " << long_try <<  ".\n";

        // for each random value of longitude and latitude, the surrounding area is checked to see if a better minimum is close by
        do {
            oldValue = value;
            for (int i = -1; i <= 1; i++) {
                for (int j = -1; j <= 1; j++) {
                    if (i == 0 && j == 0) {
                    } else {
                        // i and j are numbers between -1 and 1, so can trial a new point close by when multiplied by step
                        newValue = totalDistance(Data, lat_try + step * i, long_try + step * j);

                        // if the new value is a better minimum, i and j are saved as well as the vlaue
                        if (newValue <= value) {
                            dlat = i;
                            dlong = j;
                            value = newValue;
                        }
                    }

                }
            }
            // tests around the new point until a local minimum is found
            lat_try += step * dlat;
            long_try += step * dlong;
        } while (value < oldValue);
        // for each k value, checks if the local minimum is currently the global minimum
        // if so, saves this point as the optimum
        if (value < globalMin) {
            globalMin = value;
            gl_lat = lat_try;
            gl_long = long_try;
        }
    }

    // once co-ordinates for the global minimum are found, measures the distance to all places in GBPlaces
    double dist;
    double distnew = 1e10;
    string city;
    for (int i =0; i < Data.Long.size(); i++){

        // for each place, the Haversine distance is found
        // if this is smaller than all the previous values, its name is saved
        dist = haversine_Distance(gl_lat, Data.Lat[i], gl_long, Data.Long[i]);
        if(dist < distnew){
            distnew = dist;
            city = Data.name[i];
            }
        }
// outputs the co-ords of the optimal delivery hub and which town/ city it is closest to (for a general idea)
cout << "Optimal Co-ordinates for delivery hub: Latitude of " << gl_lat << " & Longitude of " << gl_long << ".\nThis is closest to " << city << ".";

    return 0;
}
