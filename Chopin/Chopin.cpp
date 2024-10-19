#include <iostream>
#include <string>
#include <vector>
#include <curl/curl.h>
#include <nlohmann/json.hpp> // Include the nlohmann/json header for JSON parsing
#include <cstdlib> // For rand() and srand()
#include <ctime>   // For time()

using json = nlohmann::json;

// Structure to hold a chord progression
struct ChordProgression {
    std::vector<std::string> chords;
};

// Function to handle the response from the API
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* response) {
    size_t totalSize = size * nmemb;
    response->append((char*)contents, totalSize);
    return totalSize;
}

// Function to make a request to the OpenOpus API
std::string fetchChopinWorks() {
    CURL* curl;
    CURLcode res;
    std::string response;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (curl) {
        // OpenOpus API endpoint for Chopin's works
        std::string url = "https://api.openopus.org/work/list/composer/77/genre/all.json"; // Chopin's ID is 77

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }

        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();

    return response;
}

// Function to generate a chord progression
ChordProgression generateChordProgression(const std::string& apiResponse) {
    json chopinData = json::parse(apiResponse);
    ChordProgression progression;

    // Placeholder for extracted chords
    std::vector<std::string> chopinChords = {"i", "IV", "V", "vi", "ii", "iii", "VII", "vii°7", "V7", "viio", "aug6"};

    // Ensure there is no repetition of the same chord consecutively
    int previousIndex = -1;
    srand(time(0));  // Seed random number generator

    for (int i = 0; i < 4 + rand() % 3; ++i) {  // Generate 4 to 6 chords
        int currentIndex;
        do {
            currentIndex = rand() % chopinChords.size();
        } while (currentIndex == previousIndex);  // Avoid repeating the same chord consecutively

        previousIndex = currentIndex;
        progression.chords.push_back(chopinChords[currentIndex]);
    }

    return progression;
}

int main() {
    // Fetch Chopin's works from OpenOpus API
    std::string apiResponse = fetchChopinWorks();
    
    // Generate a chord progression based on the fetched data
    ChordProgression progression = generateChordProgression(apiResponse);

    // Output the generated chord progression
    std::cout << "Generated Chord Progression based on Chopin's work: ";
    for (const auto& chord : progression.chords) {
        std::cout << chord << " ";
    }
    std::cout << std::endl;

    return 0;
}