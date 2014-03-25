#include "WilcoxonTest.h"

WilcoxonTest::WilcoxonTest(float * _data, int _dataXsize, int _dataYsize, string _testIndexes, string _controlIndexes)
{
    data = _data;
    dataXsize = _dataXsize;
    dataYsize = _dataYsize;
    testIndexes = parseIntString(_testIndexes);
    controlIndexes = parseIntString(_controlIndexes);
    if(testIndexes->size() != controlIndexes->size()){
        cout << "Control and Test indexes are not the same size!" << endl;
        cout << "This program implements paired Wilcoxon test, so please use data groups with the same size." << endl;
        throw;
    }
    readApproximatePtable();
}

WilcoxonTest::WilcoxonTest(float * _data, int _dataXsize, int _dataYsize, vector<int> * _testIndexes, vector<int> * _controlIndexes)
{
    data = _data;
    dataXsize = _dataXsize;
    dataYsize = _dataYsize;
    testIndexes = _testIndexes;
    controlIndexes = _controlIndexes;
    if(testIndexes->size() != controlIndexes->size()){
        cout << "Control and Test indexes are not the same size!" << endl;
        cout << "This program implements paired Wilcoxon test, so please use data groups with the same size." << endl;
        throw;
    }
    readApproximatePtable();
}

vector<double> * WilcoxonTest::test()
{
    pValues = new vector<double>();
    int numberOfSamples = testIndexes->size();
    for(int y = 0; y < dataYsize; y++)
    {
        int yIndex = y * dataXsize;
        float * absoluteValues = new float[numberOfSamples]();
        float * signs = new float[numberOfSamples]();
        float w = calculateWValue(yIndex, absoluteValues, signs);
        int numberOfZeroes = getNumberOfZeroes(yIndex, absoluteValues);
        pValues->push_back(calculatePValue(w, numberOfZeroes));
        delete absoluteValues;
        delete signs;
    }
    return pValues;
}

vector<int> * WilcoxonTest::parseIntString(string input)
{
    vector<int> * intVector = new vector<int>();
    vector<string> * intStrings = splitLine(input, ',');
    for(unsigned int i = 0; i < intStrings->size(); i++){
        intVector->push_back(atoi(intStrings->at(i).c_str()));
    }
    return intVector;
}

void WilcoxonTest::readApproximatePtable()
{
    string fileLocation = "/usr/lib/wilcoxonTest/pTable80.txt";
    ifstream listFile(fileLocation.c_str());
    if (!listFile.is_open())
    {
        cerr << "P values table file does not exist at " << fileLocation << endl;
        throw;
    }
    approximatePTable = new std::vector<std::vector<approximatePosition> * >;

    string sLine = "";
    while (!listFile.eof())
    {
        getline(listFile, sLine);
        if(sLine.compare("") != 0)
        {
            approximatePTable->push_back(getPositions(sLine));
        }
    }
    listFile.close();
}

std::vector<approximatePosition> * WilcoxonTest::getPositions(string positionsLine)
{
    std::vector<approximatePosition> * positions = new std::vector<approximatePosition>;
    string trimmed_input = positionsLine.substr(1, positionsLine.length() - 1);

    std::vector<string> * positionsStrings = splitLine(positionsLine, ']');
    for(unsigned int i = 0; i < positionsStrings->size(); i++){
        string rawPosistionString = positionsStrings->at(i);
        unsigned beginning = rawPosistionString.find('[');
        unsigned middle = rawPosistionString.substr(beginning).find(',');
        string x_str = rawPosistionString.substr(beginning, middle);
        string y_str = rawPosistionString.substr(middle, rawPosistionString.length());
        approximatePosition pos;
        pos.x = atoi(x_str.c_str());
        pos.y = atof(y_str.c_str());
        positions->push_back(pos);
    }

    return positions;
}

std::vector<string> * WilcoxonTest::splitLine(string inputString, char lineSplit)
{
    std::istringstream stringStream ( inputString );
    std::vector<string> * datacells = new std::vector<string>;
    while (!stringStream.eof())
    {
        string newString;
        getline( stringStream, newString, lineSplit);
        datacells->push_back(newString);
    }
    return datacells;
}

float WilcoxonTest::calculateWValue(int yIndex, float * absoluteValues, float * signs)
{
    for (unsigned int i = 0; i < testIndexes->size(); i++)
    {
        int testIndex = testIndexes->at(i);
        int controlIntex = controlIndexes->at(i);
        float x1 = data[yIndex + testIndex];
        float x2 = data[yIndex + controlIntex];

        float value = x1-x2;
        absoluteValues[i] = abs(value);
        signs[i] = getSign(value);
    }

    //Sort the list
    quicksort(0, testIndexes->size(), absoluteValues, signs);

    float * ranks = rankThePairs(yIndex, absoluteValues);
    //calculate W value
    float w = 0;
    for (unsigned int i = 0; i < testIndexes->size(); i++)
    {
        int x = testIndexes->at(i);
        if(absoluteValues[i] != 0)
        {
            w += ranks[x] * signs[i];
        }
    }

    delete ranks;
    return abs(w);
}

double WilcoxonTest::calculatePValue(float w, int numberOfZeroes)
{
    int Nr = 0;
    for (unsigned int x = numberOfZeroes; x < testIndexes->size(); x++)
    {
        Nr++;
    }
    if(Nr > 500){
        float z = calculateZValue(w, Nr);
        if(z < 0)
        {
            return gsl_cdf_gaussian_P(z, 1);
        }
        return gsl_cdf_gaussian_Q(z, 1);
    }
    else
    {
        return getApproximatePValue(w);
    }
}

float WilcoxonTest::calculateZValue(float w, int Nr)
{
    float sigma = sqrt((Nr * (Nr+1) * (2*Nr + 1)) / 6);
    return (w - 0.5) / sigma;
}

double WilcoxonTest::getApproximatePValue(float w)
{
    std::vector<approximatePosition> * approximatePositions = approximatePTable->at(testIndexes->size());
    
    approximatePosition beginningPos = approximatePositions->at(0);
    for (unsigned int i = 1; i < testIndexes->size(); i++)
    {
        approximatePosition endPos = approximatePositions->at(i);
        if (w >= beginningPos.x && w <= endPos.x){
            return approximateP(w, beginningPos, endPos);
        }
        beginningPos = endPos;
    }
    return 0;
}

double WilcoxonTest::approximateP(float w, approximatePosition beginningPos, approximatePosition endPos)
{
    double relativeValue; 
    if(w == beginningPos.x)
    {
        relativeValue = beginningPos.y;
    }
    else if (w == endPos.x)
    {
        relativeValue = endPos.y;
    }
    else
    {
        relativeValue = beginningPos.y + (endPos.y - beginningPos.y) * (w - beginningPos.x) / (endPos.x - beginningPos.x);
    }
    return relativeValue * gsl_cdf_gaussian_P(w, 1); 
}

float * WilcoxonTest::rankThePairs(int yIndex, float * absoluteValues)
{
    int testIndexesSize = testIndexes->size();

    float * ranks = new float[testIndexesSize]();

    int i = 0;
    while (i < testIndexesSize)
    {
        cout << 'h' << endl;
        int j = i + 1;
        while (j < testIndexesSize)
        {
            if(absoluteValues[i] != absoluteValues[j])
            {
                break;
            }
            j++;
        }
        for(int k = i; k <= j-1; k++)
        {
            ranks[k] = 1 + (double)(i + j-1)/(double)2;
        }

        i = j;
    }
    return ranks;
}

int WilcoxonTest::getSign(float value)
{

    if(value > 0)
    {
        return 1;
    }
    else if(value < 0)
    {
        return -1;
    }
    return 0;
}

int WilcoxonTest::getNumberOfZeroes(int yIndex, float * absoluteValues)
{
    int numberOfZeroes = 0;
    for (unsigned int i = 0; i < testIndexes->size(); i++)
    {
        if(absoluteValues[i] == 0)
        {
            numberOfZeroes++;
        }
    }
    return numberOfZeroes;
}

void WilcoxonTest::quicksort(int m, int n, float * absoluteValues, float * signs)
{
    float key;
    int i,j,k;
    if( m < n)
    {
        k = choose_pivot(m,n);
        swap(&absoluteValues[m], &absoluteValues[k]);
        swap(&signs[m], &signs[k]);
        key = absoluteValues[m];
        i = m + 1;
        j = n;
        while(i <= j)
        {
            while((i <= n) && ( absoluteValues[i] <= key))
                i++;
            while((j >= m) && ( absoluteValues[j] > key))
                j--;
            if( i < j)
            {
        swap(&absoluteValues[i], &absoluteValues[j]);
        swap(&signs[i], &signs[j]);
            }
        }
        // swap two elements
        swap(&absoluteValues[m], &absoluteValues[j]);
        swap(&signs[m], &signs[j]);
        // recursively sort the lesser list
        quicksort(m,j - 1, absoluteValues, signs);
        quicksort(j + 1, n, absoluteValues, signs);
    }
}

void WilcoxonTest::swap(float *x,float *y)
{
    float temp;
    temp = *x;
    *x = *y;
    *y = temp;
}

int WilcoxonTest::choose_pivot(int i,int j )
{
    return((i+j) /2);
}


