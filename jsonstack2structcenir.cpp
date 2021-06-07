#include "mex.hpp"         // MATLAB mandatory
#include "mexAdapter.hpp"  // MATLAB mandatory
#include <fstream>         // file stream => read file on disk
#include <regex>           // regex

#define FILENAME "jsonstack2structcenir"

std::vector<std::string> field_list = {
    "RepetitionTime",
    "EchoTime",
};

// decimal number in fixed format (99 -99. +99.999 -.999 -3 etc.)
    // prefixed by beginning of string or white space
    // with an optional + or -
    // suffixed by end of string or white space
    std::regex num_re("(?:^|\\s)[+-]?(?:\\d+\\.?\\d*|\\d*\\.?\\d+)(?:\\s|$)");

using matlab::mex::ArgumentList;
using matlab::data::ArrayType;
using matlab::data::TypedArray;
using matlab::data::Array;

// entry point of the mex
class MexFunction : public matlab::mex::Function {
    
    // File
    std::ifstream inFile; // Input stream to read from file
    std::string content;  // File content
    
    // Get pointer to engine
    std::shared_ptr<matlab::engine::MATLABEngine> matlabPtr = getEngine();
    
    // Get array factory
    matlab::data::ArrayFactory factory;
    
    // Create an output stream
    std::ostringstream stream;
    
public:
    
    /*********************************************************************/
    // simple wrappers
    void fprintf_matlab(const std::string inputString){
        stream << "[" << FILENAME << "]: " << inputString;
        matlabPtr->feval(u"fprintf", 0,
                std::vector<Array>({ factory.createScalar( stream.str() ) }));
                stream.str("");
    }
    void error_matlab(const std::string inputString){
        stream << "[" << FILENAME << "]: " << inputString;
        matlabPtr->feval(u"error", 0,
                std::vector<Array>({ factory.createScalar( stream.str() ) }));
                stream.str("");
    }
    
    /*********************************************************************/
    // Destructor
    ~MexFunction() {
        if (inFile.is_open())
            inFile.close(); // make sure the file is closed at the end of execution
    }
    
    /*********************************************************************/
    void checkArguments(ArgumentList outputs, ArgumentList inputs) {
        
        // Check  number of inputs
        if ( inputs.size() != 1) { error_matlab("Need exactly 1 input"); }
        if (outputs.size()  > 1) { error_matlab("Too many outputs"    ); }
        
        // Check nature of input
        if ( inputs[0].getType() != ArrayType::CHAR ) { error_matlab("Input must be a char"); }
        
    }
    
    /*********************************************************************/
    void readFile(std::string filename){
        
        // open or error
        inFile.open(filename);
        if (!inFile.is_open()) {
            std::ostringstream stringStream;
            stringStream << "File not opened : " << filename;
            error_matlab(stringStream.str());
        }
        
        // read file
        std::stringstream strStream;
        strStream << inFile.rdbuf(); //read the file
        content = strStream.str();
        
        // close
        inFile.close();
        
    }
    
    /*********************************************************************/
    std::string extract_line( std::regex re ){
        
        std::smatch match; // this will receive the matched elements
        
        if(std::regex_search( content, match, re )){ // search
        
            // get position of the matched postion & only keep the file from this index
            const int start = match.position();
            std::string tmp_string = content.substr(start,content.length());
            
            if(std::regex_search( tmp_string, match, std::regex (",") )){ // search for the comma which is an endline
                const int stop = match.position();
                return content.substr(start,stop); // only return the line, ready for extraction
            }
            
        }
        return std::string (""); // this an "else"
    }
    
    /*********************************************************************/
    double extract_num( std::string str, std::regex re ){
        
        std::smatch match;
        
        if(std::regex_search( str, match, re )){
            return std::stod( match[0] ) ;
        }
        return NAN;
        
    }
    
    /*===================================================================*/
    // this is the main()
    void operator()(ArgumentList outputs, ArgumentList inputs) {
        
        // Validate arguments
        checkArguments(outputs, inputs);
        
        // Get filename
        const matlab::data::CharArray filename = std::move(inputs[0]);
        readFile(filename.toAscii());
        
        // Initialize output
        matlab::data::StructArray S = factory.createStructArray({1}, field_list);
        
        // Parse file
        
        S[0]["RepetitionTime"] = factory.createScalar( extract_num( extract_line( std::regex ("RepetitionTime") ) , num_re ) );
        
        outputs[0] = std::move(S);
        
    }
    
};


