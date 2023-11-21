#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <ctime>
#include <random>

using namespace std;

void getDictionary(vector<string>& listOfWords){//this handles putting the hole wordle dictionary in vector<string> listOfWords
    string line;
    fstream file;
    file.open("dictionary/wordle-La.txt");
    if(!file.is_open()){
        cout<<"COULDN'T OPEN wordle-La.txt"<<endl;
    }
    for(int i = 0; getline(file,line);i++){
        listOfWords.push_back(line);
    }
    file.close();
}

void saveToFile(vector<string>& saveWord,int counter_for_over,vector<string>& saveLetterColor,bool is_over){//this handles saving the attempt to "all_attempts.txt"
    fstream file;
    auto start = std::chrono::system_clock::now();
    auto end = std::chrono::system_clock::now();
    std::time_t end_time = std::chrono::system_clock::to_time_t(end);
    //ctime(&end_time)
    file.open("saves/all_attempts.txt", ios::app);
    if(!file.is_open()){
        cout<<"COULD NOT OPEN SAVE FILE"<<endl;
        cout<<"ATTEMPT HAS NOT BEEN SAVED"<<endl;
        return;
    }
    if(is_over){
        file<<"------------------WON-----------------------"<<endl;
        file<<"GUESSED THE WORD "<<saveWord.back()<<endl;
        file<<"IN "<<++counter_for_over<<" GUESSES"<<endl;
    }else{
        file<<"------------------LOST-----------------------"<<endl;
    }
    file<<"TIME OF GUESSING:"<<ctime(&end_time)<<endl;
    for(int i = 0;i < saveWord.size();i++){
        file<<saveWord[i]<<endl;
        file<<saveLetterColor[i]<<endl;
        file<<endl;
    }
    file<<"--------------------------------------------"<<endl;
    file.close();
    cout<<"ATTEMPT SAVED SUCCESSFULLY"<<endl;
}

vector<string> getNewLetters(vector<string>& letters){//this is for inputting the letter colors that are given by wordle
    string letter;
    for(int i = 0;i < letters.size();i++){
        cout<<letters[i];
    }
    cout<<endl;
    cin>>letter;
    for(int i = 0;i < letter.size();i++){
        if(letter[i] >= 'a' && letter[i] <= 'z'){
            letter[i] -=' ';
        }
        letters[i] += letter[i];
    }
    return letters;
}

string toLowerCase(string text){//basic algorithm to convert string to lower case
    for(int i = 0;i < text.size();i++){
        if(text[i] >= 'A' && text[i] <= 'Z'){
            text[i] += ' ';
        }
    }
    return text;
}

int isLineAllowed(string line){//this is for the first word input if user want to put in their own word checks if that word is allowed
    if(line.size() != 5){
        return 1;
    }
    for(int i = 0; i < line.size(); i++){
        if(line[i] < 'a' || line[i] > 'z'){
            return 2;
        }
    }
    fstream file;
    file.open("dictionary/wordle-La.txt");
    if(!file.is_open()){
        cout<<"CAN NOT CHECK DICTIONARY WORD MIGHT NOT BE ALLOWED"<<endl;
        return 0;
    }
    string word;
    bool isInDictionary = false;
    while (getline(file,word)){
        if(word == line){
            isInDictionary = true;
            break;
        }
    }
    file.close();
    if(!isInDictionary){
        return 3;
    }
    return 0;
}

bool SaveLetterColor(vector<string>& saveLetterColor,vector<string>& letters){//this is to save the color of the letters
    string letter,letterColor;
    bool is_over = true;
    for(int i = 0; i < letters.size();i++){
        letter = letters[i];
        letterColor += letter[1];
        if(i + 1 >= letters.size()){
            saveLetterColor.push_back(letterColor);
        }
        if(letter[1] != 'G'){
            is_over = false;
        }
    }
    return is_over;
}

void NotLetters(vector<char>& not_letters,vector<int>& not_letters_pos,vector<string>& letters){//this is to sore the letters that are not in the word
    int size_of_a_word = 5;
    string line,letter;
    bool can_skip = false;
    for(int j = 0;j < size_of_a_word;j++){
        line = letters[j];
        for(int k = 0;k < size_of_a_word;k++){
            letter = letters[k];
            if(letter[0] == line[0]){
                if(letter[1] == 'Y' || letter[1] == 'G'){
                    can_skip = true;
                }
            }
        }
        if(can_skip)continue;
        if(line[1] == 'N'){
            not_letters.push_back(line[0]);
            not_letters_pos.push_back(j);
        }
    }
}

bool WordCheckingALG(vector<string>& letters,string line){//main algorithm for checking if a word is compatible (CAN BE MASSIVELY IMPROVED)
    /*todo:make algorithm better
     * 1.it gets confused when there is a yellow and grey of the same color it still works but not efficiently
     * 2.maybe if there are 4 greens it should instead of continuing to guess with those 4 letters-
     * -it should guess once with the remaining letters so that it eliminates more letters and makes the word easier to guess-
     * -instead of just going through the list and trying to guess every word with those 4 letters*/
    int size_of_a_word = 5;
    string letter;
    bool can_skip = false;
    for(int j = 0;j < size_of_a_word;j++){
        letter = letters[j];

        if(letter[1] == 'N')continue;//if it's an N we don't check anything

        if(letter[1] == 'G'){//if letter color is G than we want a word that has a letter in that exact position
            if(line[j] != letter[0]){
                can_skip = true;
                break;
            }
        }

        if(letter[1] == 'Y'){//if letter color is Y than we want a word with that letter in it but not in the same position
            bool has_letter = false;
            for(int k = 0;k < size_of_a_word;k++){
                if(k == j){// we don't want to check for the position where there was a yellow
                    if(line[k] != letter[0]){//if there is the same letter in the same position we skip that word
                        continue;
                    }
                    break;
                }
                if(line[k] == letter[0]){
                    has_letter = true;
                    break;
                }
            }

            if(!has_letter){
                can_skip = true;
                break;
            }
        }
    }
    return can_skip;
}

bool CheckNotLetters(vector<char>& not_letters,string line){//this checks if any of the letters of line are in not_letters if so than it skips that word
    bool can_skip = false;
    for(int j = 0;j < line.size();j++){
        for(int k = 0;k < not_letters.size();k++){
            if(line[j] == not_letters[k]){
                can_skip = true;
                break;
            }
        }
        if(can_skip)break;
    }
    return can_skip;
}


//vector<string>& not_letters,string line

//vector<string>& letters,string line
void wordle_bot(vector<string>& listOfWords,string line,const string& wantSave){//main hub of program this is where all other funcs are called
    bool is_over = false;

    int counter_for_over = 0;

    vector<char> not_letters;
    vector<int> not_letters_pos;

    vector<string> saveWord;
    vector<string> saveLetterColor;
    vector<string> letters = {"a","l","e","r","t"};

    letters[0] = line[0];
    letters[1] = line[1];
    letters[2] = line[2];
    letters[3] = line[3];
    letters[4] = line[4];

    saveWord.emplace_back(line);

    letters = getNewLetters(letters);

    SaveLetterColor(saveLetterColor,letters);

    NotLetters(not_letters,not_letters_pos,letters);

    for(int i = 0;i < listOfWords.size();i++){

        line = listOfWords[i];

        listOfWords[i] = " ";
        if(line == " "){
            continue;
        }

        if(CheckNotLetters(not_letters,line))continue;

        if(WordCheckingALG(letters,line))continue;

        counter_for_over++;
        if(counter_for_over > 5){
            counter_for_over--;
            break;
        }

        saveWord.push_back(line);

        letters[0] = line[0];
        letters[1] = line[1];
        letters[2] = line[2];
        letters[3] = line[3];
        letters[4] = line[4];

        letters = getNewLetters(letters);

        is_over = SaveLetterColor(saveLetterColor,letters);

        if(is_over)break;

        i = 0;

        NotLetters(not_letters,not_letters_pos,letters);
    }

    if(wantSave == "yes"){
        saveToFile(saveWord,counter_for_over,saveLetterColor,is_over);
    }
}

void startInput(vector<string>& listOfWords,string& line,string& wantSave){//this is for the beginning of the program for saveInfo and getting the first word
    //First inputs
    //for saves first word and getting dictionary
    string wantRandom;
    cout<<"Save attempt (yes) (no):";
    cin>>wantSave;
    cout<<"Start with random word (yes) (no):";
    cin>>wantRandom;

    wantSave = toLowerCase(wantSave);
    wantRandom = toLowerCase(wantRandom);
    string wantOwnWord;
    if(wantRandom == "no"){
        cout<<"Start with your own word (yes) (no):";
        cin>>wantOwnWord;
        wantOwnWord = toLowerCase(wantOwnWord);
    }

    getDictionary(listOfWords);

    /*todo:random stopped working for some reason
     * update:random now only works in clion compiler*/
    if(wantRandom == "yes"){
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<int> distribution(1,listOfWords.size()-1);
        int dice_roll = distribution(gen);
        line = listOfWords[dice_roll];
    }else{
        if(wantOwnWord == "yes"){
            cout<<"Word to start with:";
            cin>>line;
            line = toLowerCase(line);
            while (isLineAllowed(line) != 0){
                if(isLineAllowed(line) == 1){
                    cout<<"WORD MUST BE EXACTLY 5 LETTERS LONG:";
                    cin>>line;
                }
                if(isLineAllowed(line) == 2){
                    cout<<"WORD MUST CONTAIN ONLY LETTERS (a-z)";
                    cin>>line;
                }
                if(isLineAllowed(line) == 3){
                    cout<<"WORD NOT RECOGNIZED BY WORDLE PLEASE INPUT DIFFERENT WORD:";
                    cin>>line;
                }
            }
        }else{
            line = "alert";
        }
    }
}

int main() {
    vector<string> listOfWords;
    string line,wantSave;
    startInput(listOfWords,line,wantSave);
    wordle_bot(listOfWords,line,wantSave);
    return 0;
}
