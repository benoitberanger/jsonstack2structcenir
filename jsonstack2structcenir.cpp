#include "mex.hpp"
#include "mexAdapter.hpp"
#include <fstream>

#define FILENAME "jsonstack2structcenir"

using matlab::mex::ArgumentList;
using matlab::data::ArrayType;
using matlab::data::TypedArray;
using matlab::data::Array;

// entry point of the mex
class MexFunction : public matlab::mex::Function {
    
    // Input stream to read words from file
    std::ifstream inFile;
    
    // Get pointer to engine
    std::shared_ptr<matlab::engine::MATLABEngine> matlabPtr = getEngine();
    
    // Get array factory
    matlab::data::ArrayFactory factory;
    
public:
    
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
        inFile.open(filename);
        std::ostringstream stringStream;
        stringStream << "File not opened : " << filename;
        if (!inFile.is_open()) { error_matlab(stringStream.str()); }
    }
    
    /*********************************************************************/
    // this is the main()
    void operator()(ArgumentList outputs, ArgumentList inputs) {
        
        // Validate arguments
        checkArguments(outputs, inputs);
        
        matlab::data::CharArray char_input = std::move(inputs[0]);
        
        readFile(char_input.toAscii());
        
    }
    
    /*********************************************************************/
    // simple wrappers
    void fprintf_matlab(const std::string inputString){
        std::ostringstream stringStream;
        stringStream << "[" << FILENAME << "]: " << inputString;
        matlabPtr->feval(u"fprintf", 0,
                std::vector<Array>({ factory.createScalar( stringStream.str() ) }));
    }
    void error_matlab(const std::string inputString){
        std::ostringstream stringStream;
        stringStream << "[" << FILENAME << "]: " << inputString;
        matlabPtr->feval(u"error", 0,
                std::vector<Array>({ factory.createScalar( stringStream.str() ) }));
    }
    
};

