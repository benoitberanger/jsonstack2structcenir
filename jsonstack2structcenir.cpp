#include "mex.hpp"         // MATLAB mandatory
#include "mexAdapter.hpp"  // MATLAB mandatory
#include <fstream>         // file stream => read file on disk
#include <regex>           // regex

#define FILENAME "jsonstack2structcenir"

// entry point of the mex
class MexFunction : public matlab::mex::Function {
    
    protected:
    
    // File
    std::ifstream _inFile; // Input stream to read from file
    std::string _content;  // File content
    
    // Get pointer to engine
    std::shared_ptr<matlab::engine::MATLABEngine> _matlabPtr = getEngine();
    
    // Get array factory
    matlab::data::ArrayFactory _factory;
    
    // Create an output stream, for fprintf and error
    std::ostringstream _OUTstream;
    
    // decimal number in fixed format (99 -99. +99.999 -.999 -3 etc.)
    // prefixed by beginning of string or white space
    // with an optional + or -
    // suffixed by end of string or white space
    std::regex _num_re{"(?:^|\\s)[+-]?(?:\\d+\\.?\\d*|\\d*\\.?\\d+)(?:\\s|$)*"};
    
    /*********************************************************************/
    // simple wrappers
    void fprintf_matlab(const std::string & inputString){
        _OUTstream << "[" << FILENAME << "]: " << inputString;
        _matlabPtr->feval(u"fprintf", 0,
                std::vector<matlab::data::Array>({ _factory.createScalar( _OUTstream.str() ) }));
                _OUTstream.str("");
    }
    void error_matlab(const std::string & inputString){
        _OUTstream << "[" << FILENAME << "]: " << inputString << "\n";
        _matlabPtr->feval(u"error", 0,
                std::vector<matlab::data::Array>({ _factory.createScalar( _OUTstream.str() ) }));
                _OUTstream.str("");
    }
    
    /*********************************************************************/
    // Destructor
    ~MexFunction() {
        if (_inFile.is_open())
            _inFile.close(); // make sure the file is closed at the end of execution
    }
    
    /*********************************************************************/
    void checkArguments(matlab::mex::ArgumentList outputs, matlab::mex::ArgumentList inputs) {
        
        // Check  number of inputs
        if ( inputs.size() != 1) { error_matlab("Need exactly 1 input"); }
        if (outputs.size()  > 1) { error_matlab("Too many outputs"    ); }
        
        // Check nature of input
        if ( inputs[0].getType() != matlab::data::ArrayType::CHAR ) { error_matlab("Input must be a char"); }
        
    }
    
    /*********************************************************************/
    void readFile(const std::string & filename){
        
        // open or error
        _inFile.open(filename);
        if (!_inFile.is_open()) {
            std::ostringstream stringStream;
            stringStream << "File not opened : " << filename;
            error_matlab(stringStream.str());
        }
        
        // read file
        std::stringstream strStream;
        strStream << _inFile.rdbuf(); //read the file
        _content = strStream.str();
        
        // close
        _inFile.close();
    }
    
    /*********************************************************************/
    std::string extract_singleline( const std::string & str ){
        
        std::smatch match; // this will receive the matched elements
        if(std::regex_search( _content, match, std::regex( "\"" + str + "\": ") )){ // search
            
            // get position of the matched postion & only keep the file from this index
            const int start = match.position() + match.length();
            std::string tmp_string = _content.substr(start,_content.length());
            
            if(std::regex_search( tmp_string, match, std::regex (",") )){ // search for the comma which is an endline
                const int stop = match.position();
                return _content.substr(start,stop); // only return the line, ready for extraction
            }
            
        }
        return std::string(""); // this an "else"
    }
    
    /*********************************************************************/
    std::string extract_multiline( const std::string & str ){
        
        std::smatch match; // this will receive the matched elements
        
        if(std::regex_search( _content, match, std::regex(str) )){ // search
            
            // get position of the matched postion & only keep the file from this index
            const int start = match.position() + match.length();
            std::string tmp_string = _content.substr(start,_content.length());
            
            if(std::regex_search( tmp_string, match, std::regex ("]") )){ // search for the comma which is an endline
                const int stop = match.position();
                return _content.substr(start,stop); // only return the line, ready for extraction
            }
            
        }
        return std::string(""); // this an "else"
    }
    
    /*********************************************************************/
    double extract_num( const std::string & str ){
        
        std::smatch match;
        
        if(std::regex_search( str, match, _num_re )){
            return std::stod( match[0] ) ;
        }
        return NAN; // this an "else"
    }
    
    /*********************************************************************/
    std::string extract_str( const std::string & str ){
        
        if(str.size()){
            return str.substr(1, str.size() - 2);
        }
        return ""; // this an "else"
    }
    
    public:
    
    /*===================================================================*/
    // this is the main()
    
    void operator()(matlab::mex::ArgumentList outputs, matlab::mex::ArgumentList inputs) {
        
        // Validate arguments
        checkArguments(outputs, inputs);
        
        std::vector<std::string> field_list = {
            "MRAcquisitionType",
            "RepetitionTime",
            "EchoTime",
        };
        
        
        // Get filename
        const matlab::data::CharArray filename = std::move(inputs[0]);
        readFile(filename.toAscii());
        
        // Initialize output
        matlab::data::StructArray S = _factory.createStructArray({1}, field_list);
        
        // Parse file
        S[0]["MRAcquisitionType"] = _factory.createScalar( extract_str( extract_singleline("MRAcquisitionType") ) );
        S[0]["RepetitionTime"   ] = _factory.createScalar( extract_num( extract_singleline("RepetitionTime"   ) ) );
        S[0]["EchoTime"         ] = _factory.createScalar( extract_num( extract_multiline (      "EchoTime"   ) ) );
        
        // Finalize the output
        outputs[0] = std::move(S);
        
    }
    
};

