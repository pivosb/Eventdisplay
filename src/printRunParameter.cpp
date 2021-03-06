/*! \file printRunParameter
    \brief print run parameters from mscw or eventdisplay file to screen


*/

#include "VEvndispRunParameter.h"
#include "VTableLookupRunParameter.h"
#include "VGlobalRunParameter.h"
#include "VEvndispReconstructionParameter.h"
#include "VMonteCarloRunHeader.h"

#include "TFile.h"
#include "TTree.h"

#include <iostream>
#include <map>

using namespace std;

/*
 *  read observing direction and print N (North) or S (South)
 *
 *  works for MC and data
 *
 */
bool readObservingDirection( TFile* fIn )
{
    if( !fIn )
    {
        return false;
    }
    
    // check if this is a MC file
    VMonteCarloRunHeader* fMC = ( VMonteCarloRunHeader* )fIn->Get( "MC_runheader" );
    if( fMC )
    {
        if( TMath::Abs( fMC->az_range[0] - fMC->az_range[1] ) > 1.e-2 )
        {
            cout << "MC set cover the following range in az: ";
            cout << fMC->az_range[0] * 45. / atan( 1. ) << " - ";
            cout << fMC->az_range[1] * 45. / atan( 1. ) << endl;
        }
        else if( TMath::Abs( fMC->az_range[0] * 45. / atan( 1. ) - 0. ) < 1.e-2 )
        {
            cout << "N" << endl;
        }
        else if( TMath::Abs( fMC->az_range[0] * 45. / atan( 1. ) - 180. ) < 1.e-2 )
        {
            cout << "S" << endl;
        }
        else
        {
            cout << "ponting direction: " << fMC->az_range[0] * 45. / atan( 1. ) << endl;
        }
    }
    // read pointing direction from run parameters
    else
    {
        VEvndispRunParameter* fPar = ( VEvndispRunParameter* )fIn->Get( "runparameterV2" );
        if( fPar )
        {
            if( fPar->getObservatory_Latitude_deg() -  fPar->fTargetDec > 0. )
            {
                cout << "S" << endl;
            }
            else
            {
                cout << "N" << endl;
            }
        }
    }
    return true;
}

/*
                cout << "      -nteltypes    number of telescope types (CTA only)" << endl;
                cout << "      -ntype0, ntype1, etc  returns telescope type (LST, MST, ...) for the given index (CTA only)" << endl;
*/
bool readTelescopeTypeNumbers( TFile* fIn, string iPara )
{
    if( !fIn )
    {
        return false;
    }
    VTableLookupRunParameter* fTPar = ( VTableLookupRunParameter* )fIn->Get( "TLRunParameter" );
    if( !fTPar )
    {
        return false;
    }
    
    map<ULong64_t, unsigned int > fList_of_Tel_type;
    for( unsigned int i = 0; i < fTPar->fTelToAnalyzeData.size(); i++ )
    {
        if( fTPar->fTelToAnalyzeData[i] && fTPar->fTelToAnalyzeData[i]->fTelToAnalyze )
        {
            if( fList_of_Tel_type.find( fTPar->fTelToAnalyzeData[i]->fTelType ) != fList_of_Tel_type.end() )
            {
                fList_of_Tel_type[fTPar->fTelToAnalyzeData[i]->fTelType]++;
            }
            else
            {
                fList_of_Tel_type[fTPar->fTelToAnalyzeData[i]->fTelType] = 1;
            }
        }
        
    }
    
    if( iPara == "-nteltypes" )
    {
        cout << fList_of_Tel_type.size() << endl;
        return true;
    }
    else if( iPara.find( "ntype" ) != string::npos )
    {
        // (don't expect more than 10 different telescope types)
        unsigned teltypeIndex = atoi( iPara.substr( iPara.size() - 1, 1 ).c_str() );
        if( teltypeIndex < fList_of_Tel_type.size() )
        {
            unsigned int z = 0;
            map<ULong64_t, unsigned int >::iterator fList_of_Tel_type_iterator;
            for( fList_of_Tel_type_iterator = fList_of_Tel_type.begin();
                    fList_of_Tel_type_iterator != fList_of_Tel_type.end();
                    fList_of_Tel_type_iterator++ )
            {
                if( z == teltypeIndex )
                {
                    if( fList_of_Tel_type_iterator->first == 138704810 )
                    {
                        cout << "LST" << endl;
                    }
                    else if( fList_of_Tel_type_iterator->first == 201509515
                             || fList_of_Tel_type_iterator->first == 201309316
                             || fList_of_Tel_type_iterator->first == 909924
                             || fList_of_Tel_type_iterator->first == 201511619
                             || fList_of_Tel_type_iterator->first == 910224
                             || fList_of_Tel_type_iterator->first == 201309316
                             || fList_of_Tel_type_iterator->first == 201310418
                             || fList_of_Tel_type_iterator->first == 201511619
                             || fList_of_Tel_type_iterator->first == 201409917
                             || fList_of_Tel_type_iterator->first == 201411019 )
                    {
                        cout << "SST" << endl;
                    }
                    else if( fList_of_Tel_type_iterator->first == 10408418
                             || fList_of_Tel_type_iterator->first == 10408618 )
                    {
                        cout << "MST" << endl;
                    }
                    else if( fList_of_Tel_type_iterator->first == 207308707 )
                    {
                        cout << "MSCT" << endl;
                    }
                    else
                    {
                        cout << fList_of_Tel_type_iterator->first << endl;
                    }
                    return true;
                }
                z++;
            }
        }
        else
        {
            // not sure if a -1 as output would be better
            // (as there might be an entry with 0)
            cout << 0 << endl;
        }
    }
    
    return true;
    
}

/*
 * return number of CTA telescopes of a certain type
 *
 */
bool readNTelescopeTypes( TFile* fIn, string iPara )
{
    if( !fIn )
    {
        return false;
    }
    TTree* t = ( TTree* )fIn->Get( "telconfig" );
    if( !t )
    {
        return false;
    }
    unsigned int z = 0;
    ULong64_t       TelType = 0;
    t->SetBranchAddress( "TelType", &TelType );
    for( int i = 0; i < t->GetEntries(); i++ )
    {
        t->GetEntry( i );
        
        if( TelType == 138704810 && iPara == "-nLST" )
        {
            z++;
        }
        else if( ( TelType == 201509515
                   || TelType == 201309316
                   || TelType == 201511619
                   || 909924 ) && iPara == "-nSST" )
        {
            z++;
        }
        else if( ( TelType == 10408418
                   || TelType == 10408618 ) && iPara == "-nMST" )
        {
            z++;
        }
        else if( TelType == 207308707 && iPara == "-nMSCT" )
        {
            z++;
        }
    }
    cout << z << endl;
    return true;
    
}

bool readRunParameter( TFile* fIn, string iPara )
{
    if( !fIn )
    {
        return false;
    }
    
    VEvndispRunParameter* fPar = 0;
    
    fPar = ( VEvndispRunParameter* )fIn->Get( "runparameterV2" );
    if( !fPar )
    {
        fPar = ( VEvndispRunParameter* )fIn->Get( "runparameterDST" );
    }
    if( !fPar )
    {
        return false;
    }
    
    if( iPara == "-version" )
    {
        cout << fPar->getEVNDISP_VERSION() << endl;
    }
    else if( iPara == "-run" )
    {
        cout << fPar->frunnumber << endl;
    }
    else if( iPara == "-mcsourcefile" )
    {
        cout << fPar->fsourcefile << endl;
    }
    else if( iPara == "-date" )
    {
        cout << fPar->fDBRunStartTimeSQL << endl;
    }
    else if( iPara == "-mjd" )
    {
        cout << fPar->fDBDataStartTimeMJD << endl;
    }
    else if( iPara == "-teltoana" )
    {
        for( unsigned int i = 0; i < fPar->fTelToAnalyze.size(); i++ )
        {
            cout << fPar->fTelToAnalyze[i] + 1;
        }
        cout << endl;
    }
    else if( iPara == "-atmosphere" )
    {
        cout << fPar->fAtmosphereID << endl;
    }
    else if( iPara == "-epoch" )
    {
        cout << fPar->fInstrumentEpoch << endl;
    }
    else if( iPara == "-runtype" )
    {
        cout << fPar->fDBRunType << endl;
    }
    else if( iPara == "-evndispreconstructionparameterfile" )
    {
        cout << fPar->freconstructionparameterfile << endl;
    }
    else if( iPara == "-runinfo" )
    {
        cout << fPar->fInstrumentEpoch << "\t";
        cout << fPar->fAtmosphereID << "\t";
        cout << fPar->fDBRunType << "\t";
        for( unsigned int i = 0; i < fPar->fTelToAnalyze.size(); i++ )
        {
            cout << fPar->fTelToAnalyze[i] + 1;
        }
        cout << endl;
    }
    
    return true;
}


bool readMCParameter( TFile* fIn, string iPara )
{
    if( !fIn )
    {
        return false;
    }
    
    VMonteCarloRunHeader* fMC = 0;
    
    fMC = ( VMonteCarloRunHeader* )fIn->Get( "MC_runheader" );
    if( !fMC )
    {
        return false;
    }
    
    if( iPara == "-mcaz" )
    {
        fMC->printMCAz();
    }
    else if( iPara == "-runnumber" )
    {
        fMC->printRunNumber();
    }
    else if( iPara == "-MCruninfo" )
    {
            cout << "RUN " << fMC->runnumber;
            cout << " Ze " << 90.-fMC->alt_range[0]*TMath::RadToDeg();
            cout << " " << 90.-fMC->alt_range[1]*TMath::RadToDeg();
            cout << " Az " << fMC->az_range[0]*TMath::RadToDeg();
            cout << " " << fMC->az_range[1]*TMath::RadToDeg();
            cout << " Primary " << fMC->primary_id;
            cout << " NShower " << fMC->num_showers << " " << fMC->num_use;
            cout << " HighE details " << fMC->corsika_high_E_detail;
            cout << endl;
    }
    else
    {
        return false;
    }
    
    return true;
}

////
// print help screen
void printHelp()
{
    cout << endl;
    cout << "printRunParameter " << VGlobalRunParameter::getEVNDISP_VERSION() << endl;
    cout << "==========================" << endl;
    cout << endl;
    cout << "usage: printRunParameter <file> [opt]" << endl;
    cout << endl;
    cout << "print run parameters stored in eventdisplay or mscw_energy file" << endl;
    cout << endl;
    cout << "   options: " << endl;
    cout << "      -version      print Evndisp version" << endl;
    cout << "      -mcaz         print MC azimuth angle" << endl;
    cout << "      -runnumber    print MC run number" << endl;
    cout << "      -mcsourcefile print source file name" << endl;
    cout << "      -date         print date of run" << endl;
    cout << "      -mjd          print mjd of run" << endl;
    cout << "      -epoch        print epoch of this run" << endl;
    cout << "      -atmosphere   print corsika ID of atmospheric condition of this run" << endl;
    cout << "      -runtype      print run type, eg observing, obsFilter etc." << endl;
    cout << "      -teltoana     print telescope combination used in analysis" << endl;
    cout << "      -evndispreconstructionparameterfile print evndisp reconstruction parameter file" << endl;
    cout << "      -runinfo      print relevant run info in one line" << endl;
    cout << "      -MCruninfo    print relevant info on MC run in one line" << endl;
    cout << "      -nLST, -nSST, -nMST, -nMSCT number of telescopes for a specific telescope type (CTA only)" << endl;
    cout << "      -nteltypes    number of telescope types (CTA only)" << endl;
    cout << "      -ntype0, ntype1, etc  returns telescope type (LST, MST, ...) for the given index (CTA only)" << endl;
    cout << "      -obsdir       print observing direction (N=North or S=South)" << endl;
    cout << endl;
    exit( EXIT_SUCCESS );
    
}

int main( int argc, char* argv[] )
{
    // print version only
    if( argc == 2 )
    {
        string fCommandLine = argv[1];
        if( fCommandLine == "-v" || fCommandLine == "--version" )
        {
            VGlobalRunParameter fRunPara;
            cout << fRunPara.getEVNDISP_VERSION() << endl;
            exit( EXIT_SUCCESS );
        }
        else if( fCommandLine == "-h" || fCommandLine == "--help" )
        {
            printHelp();
        }
    }
    
    if( argc != 2 && argc != 3 )
    {
        printHelp();
    }
    // command line option
    string fOption = "";
    if( argc == 3 )
    {
        fOption = argv[2];
    }
    if( fOption.size() == 0 )
    {
        cout << endl;
        cout << "printRunParameter " << VGlobalRunParameter::getEVNDISP_VERSION() << endl;
        cout << "==========================" << endl;
    }
    
    // open file
    TFile* fIn = new TFile( argv[1] );
    if( fIn->IsZombie() )
    {
        cout << "error: file not found: " << argv[1] << endl;
        cout << "exiting..." << endl;
        exit( EXIT_SUCCESS );
    }
    
    if( fOption.size() > 0 )
    {
        if( fOption == "-mcaz" || fOption == "-runnumber" || fOption == "-MCruninfo" )
        {
            readMCParameter( fIn, fOption );
        }
        else if( fOption == "-nLST" || fOption == "-nMST" || fOption == "-nSST" || fOption == "-nMSCT" )
        {
            readNTelescopeTypes( fIn, fOption );
        }
        else if( fOption == "-nteltypes" || fOption.find( "ntype" ) != string::npos )
        {
            readTelescopeTypeNumbers( fIn, fOption );
        }
        else if( fOption == "-obsdir" )
        {
            readObservingDirection( fIn );
        }
        else
        {
            readRunParameter( fIn, fOption );
        }
        exit( EXIT_SUCCESS );
    }
    
    VEvndispRunParameter* fPar = 0;
    
    fPar = ( VEvndispRunParameter* )fIn->Get( "runparameterV2" );
    if( !fPar )
    {
        fPar = ( VEvndispRunParameter* )fIn->Get( "runparameterDST" );
    }
    
    if( fPar )
    {
        cout << "reading eventdisplay runparameter for version ";
        cout << fPar->GetTitle();
        cout << " (" << fPar->fEventDisplayUser << ")" << endl;
        if( fPar->fEventDisplayUser != "CTA-DST" )
        {
            fPar->print( 2 );
        }
        else
        {
            fPar->printCTA_DST();
        }
    }
    
    // array analysis cuts
    
    VEvndispReconstructionParameter* fArrayCuts = 0;
    
    fArrayCuts = ( VEvndispReconstructionParameter* )fIn->Get( "EvndispReconstructionParameter" );
    if( fArrayCuts )
    {
        cout << endl << endl;
        cout << "===========================================" << endl;
        cout << "===========================================" << endl;
        fArrayCuts->print_arrayAnalysisCuts();
    }
    
    VTableLookupRunParameter* fTPar = 0;
    
    fTPar = ( VTableLookupRunParameter* )fIn->Get( "TLRunParameter" );
    
    if( fTPar )
    {
        cout << endl << endl;
        cout << "===========================================" << endl;
        cout << "===========================================" << endl;
        fTPar->print( 2 );
    }
    
    VMonteCarloRunHeader* fMC = 0;
    
    //    if( fPar && fPar->fEventDisplayUser != "CTA-DST" )
    {
        fMC = ( VMonteCarloRunHeader* )fIn->Get( "MC_runheader" );
        if( fMC )
        {
            cout << endl << endl;
            cout << "===========================================" << endl;
            cout << "===========================================" << endl;
            fMC->print();
        }
    }
    
    
    fIn->Close();
    
}
