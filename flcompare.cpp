#include <iostream>
#include <fstream>
#include <vector>
#include <boost/regex.hpp>

#define DEFAULT_PROGRESS_THROTTLE 100

int main(int argc, char *argv[]){
    bool failed = false;
    bool whole_word = true;
    bool case_sensitive = false;
    int progress_throttle = DEFAULT_PROGRESS_THROTTLE;
    std::string strFileFound;
    std::string strFileNotFound;
    std::fstream fsIn;
    std::fstream fsOut;
    std::fstream fsOutFound;
    std::fstream fsOutNotFound;
    std::vector<std::string> vSource;
    std::vector<std::string> vCompare;
    std::vector<std::string> vFound;
    
    // check args...
    if (argc < 3 || argv[1] == 0 || argv[2] == 0){
        std::cout << "Files Partial Line Compare args:\n" <<
        "\t<source_file>\n"  <<
        "\t<compare_file>\n" <<
        "\t<optional bool: compare as whole words (def. 1)>\n" <<
        "\t<optional bool: compare is case-sensitive (def. 0)>\n" <<
        "\t<optional int: progress update throttle (def. 100)>\n";
        failed = true;
    }

    // open source/compare files...
    if (!failed){
        strFileFound = std::string(argv[2]) + ".found.txt";
        strFileNotFound = std::string(argv[2]) + ".not_found.txt";
        fsIn.open(argv[1],  std::fstream::in);
        fsOut.open(argv[2], std::fstream::in);
        if (fsIn.fail()){
            std::cout << "Could not open source file: " << argv[1];
            failed = true;
        } else if (fsOut.fail()) {
            std::cout << "Could not open compare file: " << argv[2];
            failed = true;
        }
    }
        
    // open output files...
    if (!failed){
        strFileFound = std::string(argv[2]) + ".found.txt";
        strFileNotFound = std::string(argv[2]) + ".not_found.txt";
        
        // truncate file first (if exists)...
        fsOutFound.open(strFileFound,       std::fstream::out | std::fstream::trunc);
        fsOutNotFound.open(strFileNotFound, std::fstream::out | std::fstream::trunc);
        if (fsOutFound.fail()){
            std::cout << "Could not truncate output file: " << strFileFound;
            failed = true;
        } else if (fsOutNotFound.fail()){
            std::cout << "Could not truncate output file: " << strFileNotFound;
            failed = true;
        }
        fsOutFound.close();        
        fsOutNotFound.close();
        
        // now open for write/append...
        if (!failed){
            fsOutFound.open(strFileFound,       std::fstream::out | std::fstream::app);
            fsOutNotFound.open(strFileNotFound, std::fstream::out | std::fstream::app);
            if (fsOutFound.fail()){
                std::cout << "Could not open output file: " << strFileFound;
                failed = true;
            } else if (fsOutNotFound.fail()){
                std::cout << "Could not open output file: " << strFileNotFound;
                failed = true;
            }
        }

        // get vectors...   
        if (!failed){
            std::string line;
            while (std::getline(fsIn, line))
                vSource.push_back(line);
            while (std::getline(fsOut, line))
                vCompare.push_back(line);

            std::cout << "\nSource  File: " << argv[1] << " (" << vSource.size()  << " lines)" << "\n";
            std::cout << "Compare File: " << argv[2] << " (" << vCompare.size() << " lines)" << "\n";
            if (vSource.size() == 0){
                std::cout << "Empty source file found: " << argv[1];
                failed = true;
            } else if (vCompare.size() == 0){
                std::cout << "Empty compare file found: " << argv[2];    
                failed = true;
            }
            
            // close early since we preloaded into memory...
            fsIn.close();
            fsOut.close();
        }
        
        // whole words optional arg...
        if (argc > 3)
            whole_word = (bool) strtol(argv[3], NULL, 10);
        std::cout << "Whole Words: " << (whole_word ? "yes" : "no") << "\n";

        if (argc > 4)
            case_sensitive = (bool) strtol(argv[4], NULL, 10);
        std::cout << "Case-Sensitive: " << (case_sensitive ? "yes" : "no") << "\n";
        
        // progress throttle optional arg...
        if (argc > 5)
            progress_throttle = strtol(argv[5], NULL, 10);
        std::cout << "Progress Throttle: every (" << progress_throttle << ") source lines\n";
    }
        
    // compare strings and write to file...
    if (!failed){
        unsigned int lc_in = 1;
        boost::regex expr;
        std::string progress;
        std::cout << "\nProgress: ";
        
        for (std::vector<std::string>::iterator itIn = vSource.begin(); itIn != vSource.end(); ++itIn){
            if (whole_word)
                 expr = boost::regex(std::string(".*\\b" + *itIn + "\\b.*").c_str(), case_sensitive ? 0 : boost::regex::icase);
            else expr = boost::regex(std::string(".*"    + *itIn + ".*").c_str(),    case_sensitive ? 0 : boost::regex::icase);
                
            // don't break when found here in case there are other lines that also match...
            for (std::vector<std::string>::iterator itOut = vCompare.begin(); itOut != vCompare.end(); ++itOut)
                if ((*itOut).length() && boost::regex_match(*itOut, expr)){
                    vFound.push_back(*itOut);
                    (*itOut) = "";
                }
            
            // update progress: erase current progress first...
            if (lc_in % progress_throttle == 0 || lc_in == 1){
                for (size_t t = 0; t < progress.length(); ++t)
                    std::cout << "\b";
                progress = std::to_string(lc_in) + "/" + std::to_string(vSource.size());
                std::cout << progress;
            }
            
            lc_in++;
        }
        
        // write found...
        for (std::vector<std::string>::iterator itFound = vFound.begin(); itFound != vFound.end(); ++itFound)
            fsOutFound << (*itFound) + "\n";
        
        // write not found (original vector with found lines blanked out)...
        unsigned int lines_not_found = 0;
        for (std::vector<std::string>::iterator itOut = vCompare.begin(); itOut != vCompare.end(); ++itOut)
            if ((*itOut).length()){
                 fsOutNotFound << (*itOut) + "\n";
                 lines_not_found++;
            }
        
        // output results...
        std::cout << 
            "\n\n----------------\n" <<
            "Compare Complete\n" << 
            "----------------\n" <<
            strFileFound << ": " << vFound.size() << " lines\n" <<
            strFileNotFound << ": " << lines_not_found << " lines\n";
    }
        
    fsIn.close();
    fsOut.close();
    fsOutFound.close();
    fsOutNotFound.close();
    return (int) failed;
}
