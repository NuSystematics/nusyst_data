#include "weightAMUvariations.h"
#include <string>   //cout and fixed
#include <iomanip>  //setprecision
#include "TFile.h"
#include "TH2.h"

/*!
 *  Code example to extract the AMU DIS effects weights
 *  ratio uncertainties and absolute XS from the prepared files.
 *  Based originally on RPA code for the Valencia model in MINERvA by
 *  Heidi Schellman (Oregon State) and Rik Gran (Minnesota Duluth)
 *  for use in MINERvA experiment analysis
 *    The underlying models are from the AMU group and GENIE's Bodek Yang implementation
 *    see the .h file for more documentation and references.
 *  must compile with the ROOT libraries
 *  g++ `root-config --glibs --cflags` -O3 weightAMUDIStest.cxx -o weightAMUDIStest
 */

int main(int argc, char **argv){

  if(argc <= 1){
    std::cout << "usage: weightAMUDIStest x y (Enu, nu/antinu) " << std::endl;
    std::cout << "or:    weightAMUDIStest --all [ &> out.txt OR | less ]" << std::endl;
    return 1;
  }
  
  double x = 0.1;
  double y = 10.0;

  double MN = 0.5 * (0.93827208943 + 0.93956542052);
  
  // new code from Rik

  std::string directory = "./";
  //weightRPA myRPAweights = weightRPA(directory+"outNievesRPAratio-anu12C-20GeV-20170202.root");
  weightAMUvariations myAMUvariations = weightAMUvariations("./F1txtfile.txt","./F2txtfile.txt", "./F3txtfile.txt");

  std::cout << "Class instantiated " << std::endl;
  
  // this will hold central value, +/- suppression, +/- "enhancement"
  double weights[10] = {1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0};

  double mlep = 0.1056583755;
  double Enu = 10.0;
  int antinu = 0;
  
  if(argc >= 3){
    x = atof(argv[1]);
    y = atof(argv[2]);
    Enu = atof(argv[3]);
    antinu = atoi(argv[4]);

    std::cout << "Start getWeight " << std::endl;
    double testthird = myAMUvariations.getWeight(x,y,Enu, mlep, antinu, weights);
    //double testthird = myAMUvariations.getAbsolute(x,y,Enu, mlep, antinu, weights);

    double thisQ2 = 2.0 * MN * x * y * Enu;
    
    std::cout << " test inputq0q3 " //<< std::setprecision(4) << std::fixed
	      << " x= " << x << " y= " << y << " Q2= " << thisQ2 
	      << " w5 " << weights[0]
	      << " " << weights[1] << " " << weights[2]
	      << " " << weights[3] << " " << weights[4] << " " << weights[5]
      	      << " " << weights[6] << " " << weights[7] << " " << weights[8] << " " << weights[9]
	      << std::endl;

    testthird = myAMUvariations.getAbsolute(x, y, Enu, mlep, antinu, weights);
    std::cout << " test absolute " //<< std::setprecision(4) << std::fixed
	      << " x= " << x << " y= " << y << " Q2= " << thisQ2
	      << " w5 " << weights[0]
	      << " " << weights[1] << " " << weights[2]
	      << " " << weights[3] << " " << weights[4] << " " << weights[5]
	      << " " << weights[6] << " " << weights[7] << " " << weights[8] << " " << weights[9]
	      << std::endl;

    if(0){
    testthird = myAMUvariations.getIntegral(Enu, mlep, antinu, weights);
    std::cout << " test integral " //<< std::setprecision(4) << std::fixed
	      << " x= " << x << " y= " << y << " Q2= " << thisQ2
	      << " w5 " << weights[0]
	      << " " << weights[1] << " " << weights[2]
	      << " " << weights[3] << " " << weights[4] << " " << weights[5]
	      << " " << weights[6] << " " << weights[7] << " " << weights[8] << " " << weights[9]
	      << std::endl;
    }

    if(0){
      TFile myfile("histos.root","recreate");
      TH2D *hAMU = new TH2D("hAMU","hAMU",100,0,1,100,0,1);
      hAMU->GetXaxis()->SetTitle("Bjorken x");  hAMU->GetYaxis()->SetTitle("inelasticity y");
      TH2D *hG = new TH2D("hG","hG",100,0,1,100,0,1);
      hG->GetXaxis()->SetTitle("Bjorken x");  hG->GetYaxis()->SetTitle("inelasticity y");
      
      TH2D *hQ2 = new TH2D("hQ2","hQ2",100,0,1,100,0,1);
      TH2D *hW = new TH2D("hW","hW",100,0,1,100,0,1);

      
      
      for(double y = 0.005; y < 1.0; y+=0.01){
	for(double x = 0.005; x < 1.0; x += 0.01){

	  double temp = myAMUvariations.getAbsolute(x,y, Enu, mlep, antinu, weights);
	  //hAMU->Fill(x,y,log10(weights[3]));
	  //hG->Fill(x,y,log10(weights[7]));
	  hAMU->Fill(x,y,weights[3]);
	  hG->Fill(x,y,weights[7]);
	  double thisQ2 = 2.0 * MN * x * y * Enu;
	  //double thisW = MN * MN + 2.0 * MN * y * Enu * (1 - x);
	  double thisW2 = MN * MN + 2.0 * MN * y * Enu - thisQ2;
	  hQ2->Fill(x,y,thisQ2);
	  hW->Fill(x,y,sqrt(thisW2)); 
	}
       
	  //std::cout << "Exy " << Enu << " " << x << " " << y << " " << weights[0] << " " << weights[1] << " " << weights[2] << " " << weights[3] << " " << weights[4] << " " << weights[5] << " " << weights[6] << " " << weights[7] << " " << weights[8] <<  std::endl;
      //}   	
      }
      hAMU->Write();
      hG->Write();
      hQ2->Write();
      hW->Write();
      myfile.Clear();
      myfile.Close();
      
    }


    if(1){
      for(double y = 0.05; y < 1.0; y+=0.05){
	//double x = 0.1;
	double temp = myAMUvariations.getAbsolute(x,y, Enu, mlep, antinu, weights);
	//double temp = myAMUvariations.getWeight(x,y, Enu, mlep, antinu, weights);
	//double temp = myAMUvariations.getF2(x,y, Enu, mlep, antinu, weights);
      //if(weights[1] > 50. || weights[1] < 0.1){
      //if(weights[1] > 20.){
	double thisQ2 = 2.0 * MN * x * y * Enu;
	std::cout << "ExyQ2 " << Enu << " " << x << " " << y << " " << thisQ2 << " " << weights[0] << " " << weights[1] << " " << weights[2] << " " << weights[3] << " " << weights[4] << " " << weights[5] << " " << weights[6] << " " << weights[7] << " " << weights[8] << " " << weights[9] <<  std::endl;
      //}   	
      }
    }


  }

  return 0;
}
