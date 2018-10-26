/* Copyright 2018 Kristofer Björnson
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "TBTK/Model.h"
#include "TBTK/Plotter.h"
#include "TBTK/PropertyExtractor/BlockDiagonalizer.h"
#include "TBTK/Solver/BlockDiagonalizer.h"
#include "TBTK/Streams.h"

using namespace std;
using namespace TBTK;
using namespace Plot;

Model createModel1D(){
	//Parameters.
	const int SIZE_X = 10000;
	double t = 1;

	//Create the Model.
	Model model;
	for(int x = 0; x < SIZE_X; x++){
		double KX = 2*M_PI*x/(double)SIZE_X - M_PI;
		model << HoppingAmplitude(
			-2*t*cos(KX),
			{x},
			{x}
		);
	}
	model.construct();

	return model;
}

Model createModel2D(){
	//Parameters.
	const int SIZE_X = 500;
	const int SIZE_Y = 500;
	double t = 1;

	//Create the Model.
	Model model;
	for(int x = 0; x < SIZE_X; x++){
		for(int y = 0; y < SIZE_Y; y++){
			double KX = 2*M_PI*x/(double)SIZE_X - M_PI;
			double KY = 2*M_PI*y/(double)SIZE_Y - M_PI;
			model << HoppingAmplitude(
				-2*t*(cos(KX) + cos(KY)),
				{x, y},
				{x, y}
			);
		}
	}
	model.construct();

	return model;
}

Model createModel3D(){
	//Parameters.
	const int SIZE_X = 200;
	const int SIZE_Y = 200;
	const int SIZE_Z = 200;
	double t = 1;

	//Create the Model.
	Model model;
	for(int x = 0; x < SIZE_X; x++){
		for(int y = 0; y < SIZE_Y; y++){
			for(int z = 0; z < SIZE_Z; z++){
				double KX = 2*M_PI*x/(double)SIZE_X - M_PI;
				double KY = 2*M_PI*y/(double)SIZE_Y - M_PI;
				double KZ = 2*M_PI*z/(double)SIZE_Z - M_PI;
				model << HoppingAmplitude(
					-2*t*(cos(KX) + cos(KY) + cos(KZ)),
					{x, y, z},
					{x, y, z}
				);
			}
		}
	}
	model.construct();

	return model;
}

int main(int argc, char **argv){
	//Filenames to save the figures as.
	string filenames[3] = {
		"figures/DOS_1D.png",
		"figures/DOS_2D.png",
		"figures/DOS_3D.png"
	};

	//Loop over 1D, 2D, and 3D.
	for(int n = 0; n < 3; n++){
		//Create the Model.
		Model model;
		switch(n){
		case 0:
			model = createModel1D();
			break;
		case 1:
			model = createModel2D();
			break;
		case 2:
			model = createModel3D();
			break;
		default:
			Streams::out << "Error: Invalid case value.\n";
			exit(1);
		}

		//Setup and run the Solver.
		Solver::BlockDiagonalizer solver;
		solver.setModel(model);
		solver.run();

		//Setup the PropertyExtractor.
		PropertyExtractor::BlockDiagonalizer propertyExtractor(solver);
		propertyExtractor.setEnergyWindow(-7, 7, 1000);
		Property::DOS dos = propertyExtractor.calculateDOS();

		//Normalize the DOS.
		for(unsigned int c = 0; c < dos.getResolution(); c++)
			dos(c) = dos(c)/model.getBasisSize();

		//Plot and save the result.
		Plotter plotter;
		plotter.setLabelX("Energy");
		plotter.setLabelY("DOS");
		plotter.plot(dos, 0.05, 101);
		plotter.save(filenames[n]);
	}

	return 0;
}
