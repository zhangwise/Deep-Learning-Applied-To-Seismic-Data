// Author: Aliaksei Mikhailiuk, 2017.

#include "../headers/OutWriter.h"

int OutWriter::write(NeuralNetwork * neuralNetwork,TrainingAlgorithm * algorithm, double time, std::string mark)
{
        // Generate the folder name
	getFolderName();
        
        // Combine with the path where want to solve
	folderName = algorithm->getNameSaveFolder()+folderName+mark;
        
        // Convert to a required format and create
	const char *folderNameChar = folderName.c_str();
	const int dir_err = mkdir(folderNameChar,S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

        // If cannot create a directory
	if (-1 == dir_err)
	{
                std::cout<<"Error creating directory!"<<" "<<folderName<<" "<<dir_err<<"\n";
    	        exit(1);
	}

        // Function to write a learning rate file
	writeLearningRate(algorithm);

        // Write the generated cubes into a folder
        algorithm->writeOutput(folderName);

        // Write the evolution of the training error
	writeErrorTrain(algorithm);

        // Write the evolution of the validation error
	writeErrorValidation(algorithm);

        // Write ratio 
	writeRatio(algorithm);

        // Wrigths weights into a file
	writeWeights(neuralNetwork,algorithm);

        // Write the information about the time spent on the functions
        writeTimeInfo(neuralNetwork,algorithm,time);

        // Write the statistics about neuron acitvations
        writeStats(neuralNetwork,algorithm);

        // Copy the config file to a folder as well
        std::string outputConf = folderName+"/config.cfg";
        std::ifstream  src("./config/config.cfg", std::ios::binary);
        std::ofstream  dst(outputConf,   std::ios::binary);

        dst << src.rdbuf();

        std::cout << "Output is saved to " << folderName<<std::endl << "To visualise type: make plot" << std::endl;
	return 0;
}


int OutWriter::writeTimeInfo(NeuralNetwork *neuralNetwork,TrainingAlgorithm *algorithm, double time){
 
        // Create a file
	std::ofstream write_file;

        std::string fileName = folderName+"/time.dat";
        write_file.open(fileName,  std::ios_base::out);

        // Write the setting used for the neural network 
        write_file << algorithm->getNumbEpoches() << " "
                   << algorithm->getNumbItTrain() << " "
                   << neuralNetwork->getMiniBatch()    << " "
                   << neuralNetwork->getNumbLayers()   << " ";
        for (int ii = 0; ii < neuralNetwork->getNumbLayers(); ++ii){
                write_file << neuralNetwork->getLayer(0,ii)->getNumbUnits() << " ";
        }
        
        // Write time required for the neural network with this setting
        write_file << time << " "
		   << algorithm->getTimeFeedForward()   << " "
		   << algorithm->getTimeDeltaCompute()  << " "
		   << algorithm->getTimeBackpropagate() << " ";

	write_file<<"\n";
	write_file.close();
	return 0;
}

int OutWriter::writeLearningRate(TrainingAlgorithm *algorithm)
{

        // Create a file for learning rate
	std::ofstream write_file;

	std::string fileName = folderName+"/learning.dat";
	write_file.open(fileName,  std::ios_base::out);

        // Write the learning rates into a file
	for (int ii = 0; ii < algorithm->getLearningRateArrSize(); ++ii){
		write_file << algorithm->getLearningRateArr(ii) << " ";
	}
	write_file<<"\n";
	write_file.close();
	return 0;
}



int OutWriter::getFolderName(){


        time_t rawtime;
        struct tm * timeinfo;
        char buffer[80];

        time (&rawtime);
        timeinfo = localtime(&rawtime);

        strftime(buffer,80,"%d_%m_%Y_%I_%M_%S",timeinfo);
        std::string tmp(buffer);

        std::random_device rd;
        std::mt19937 gen(rd());

        std::uniform_int_distribution<> dis(1, 1000);

        std::string s = std::to_string(dis(gen));
        folderName = tmp+s;

        return 0;
}


int OutWriter::writeErrorValidation(TrainingAlgorithm *algorithm){

        // Create a file to write validation error
        std::ofstream write_file;
	
        std::string fileName = folderName+"/errorValidation.dat";
	
        write_file.open(fileName,  std::ios_base::out);

        // Iterate over all exemplars and get the validation error
        for (int ii = 0; ii < algorithm->getErrorValidationSize(); ++ii){
                write_file << algorithm->getErrorValidation(ii) << "\n"; 
        }

        write_file.close();
        return 0;
}

int OutWriter::writeErrorTrain(TrainingAlgorithm *algorithm){

        // Create a file for the training errror
	std::ofstream write_file;
	
	std::string fileName = folderName+"/errorTrain.dat";
	
	write_file.open(fileName,  std::ios_base::out);

	for (int ii = 0; ii < algorithm->getErrorTrainSize(); ++ii){
		write_file << algorithm->getErrorTrain(ii) << "\n"; 
	}

	write_file.close();
	return 0;
}

int OutWriter::writeRatio(TrainingAlgorithm *algorithm){

        // Create the file for validation error slope
	std::ofstream write_file;
	
	std::string fileName = folderName+"/ratio.dat";
	
	write_file.open(fileName,  std::ios_base::out);

	for (int ii = 0; ii < algorithm->getRatioSize(); ++ii){
		write_file << algorithm->getRatio(ii) << "\n"; 
	}

	write_file.close();
	return 0;
}

int OutWriter::writeStats (NeuralNetwork *neuralNetwork, TrainingAlgorithm *algorithm){

        // If the flag for the stats was active
        if (neuralNetwork->getStatsFlag() == 1){

                
                std::ofstream write_file;
        	std::string fileName = folderName+"/stats.dat";
        	write_file.open(fileName,  std::ios_base::out);
                      
                // Write the info about layer sizes
                for (int ii = 1; ii < neuralNetwork->getNumbLayers()-2; ++ii){
                        write_file << neuralNetwork->getLayer(0,ii)->getNumbUnits() << " ";
                }

                // Printed separately because of the new line character at the end
                write_file << neuralNetwork->getLayer(0,neuralNetwork->getNumbLayers()-2)->getNumbUnits() << "\n";
                
                // Write the stats
                for (int ii = 0; ii < neuralNetwork->getNumbLayers()-2; ++ii){
                        for (int jj = 0; jj < neuralNetwork->getLayer(0,ii+1)->getNumbUnits(); ++jj){
		                write_file << neuralNetwork->getStats(ii, jj) << " ";
                        }
                        write_file<<"\n";    
                }


                write_file.close();
        }
        return 0;
}

int OutWriter::writeWeights(NeuralNetwork * neuralNetwork,TrainingAlgorithm *algorithm){

	std::ofstream write_file;

        // Go through all layers
	for( int ii = 0; ii < neuralNetwork->getNumbLayers()-1; ++ii){
	
                // Create the write file for every layer of weights
        	std::string s = std::to_string(ii);
		std::string fileName = folderName+"/weights"+s+".dat";
		write_file.open(fileName,  std::ios_base::out);

                // Write the weight dimentions
		write_file<< neuralNetwork->getLayer(0,ii)->getNumbUnits()+1 << " "
                          << neuralNetwork->getLayer(0,ii+1)->getNumbUnits() << " "
                          << algorithm->getImageDepth()  << " "
                          << algorithm->getImageHeight() << " "
                          << algorithm->getImageWidth()  << "\n";


                // Write out weights layer by layer
		for (int jj = 0; jj<neuralNetwork->getLayer(0,ii+1)->getNumbUnits(); ++jj){

			for (int kk= 0; kk<neuralNetwork->getLayer(0,ii)->getNumbUnits(); ++kk){

		   		write_file<<neuralNetwork->getWeightO(ii,jj,kk)<<" ";
		        }

		        write_file << neuralNetwork->getWeightO(ii,jj,neuralNetwork->getLayer(0,ii)->getNumbUnits()); //no need for a space after last print thus outside for loop
		        write_file << "\n";
		}

		write_file.close();
	}
	return 0;
}
