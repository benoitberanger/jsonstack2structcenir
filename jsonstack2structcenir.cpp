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
    
    // File
    std::ifstream inFile; // Input stream to read from file
    std::string content;  // File content
    
    // Get pointer to engine
    std::shared_ptr<matlab::engine::MATLABEngine> matlabPtr = getEngine();
    
    // Get array factory
    matlab::data::ArrayFactory factory;
    
public:
    
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
    // this is the main()
    void operator()(ArgumentList outputs, ArgumentList inputs) {
        
        // Validate arguments
        checkArguments(outputs, inputs);
        
        const matlab::data::CharArray char_input = std::move(inputs[0]);
        readFile(char_input.toAscii());
        
        // fprintf_matlab(content);
        
        outputs[0] = factory.createCharArray(content);
        
    }
    
};

