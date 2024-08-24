if [ -z "$1" ]; then
    echo "Please provide the folder name"
    exit 1
fi

if [ -z "$2" ]; then
    echo "Please provide the test file name"
    exit 1
fi

FOLDER=$1
TEST_FILE="inputs/$FOLDER/tests/$2"

if [ ! -d "./inputs/$FOLDER" ]; then
    echo "Folder '$FOLDER' does not exist."
    exit 1
fi

if [ ! -f "./$TEST_FILE" ]; then
    echo "File '$TEST_FILE' does not exist."
    exit 1
fi

FILE="inputs/$FOLDER/asm.s"

cd assembler && g++ parser.cpp -o parser && ./parser "../$FILE" && rm parser && cd ../executor && g++ interpreter.cpp -o interpreter && ./interpreter < "../$TEST_FILE" && rm interpreter && cd ..

