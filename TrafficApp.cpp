#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <ctime>
#include <iomanip>

using namespace std;

// Forward declarations
class TrafficPoliceApp;
class Public;

class TrafficViolation 
{
public:
    enum ViolationType 
    {
        SPEEDING,
        RED_LIGHT,
        NO_SEAT_BELT,
        INVALID_LICENSE
    };
};

class Challan 
{
    // Define Challan class for storing challan information
};

class User 
{
protected:
    string username;
    string password;

public:
    User(const string& uname, const string& pwd) : username(uname), password(pwd) {}

    virtual void showMenu() = 0;
    virtual void viewProfile() const = 0;
    const string& getUsername() const 
    {
        return username;
    }
    const string& getPassword() const 
    {
        return password;
    }
};

class Public : public User 
{
private:
    string licenseNumber;
    bool hasAppointment;
    bool hasChallan;
    time_t appointmentTime; // New member to store appointment time
    TrafficPoliceApp& trafficPoliceApp;  // Reference to TrafficPoliceApp

public:
    Public(const string& uname, const string& pwd, const string& license, TrafficPoliceApp& tpa)
        : User(uname, pwd), licenseNumber(license), hasAppointment(false), hasChallan(false), appointmentTime(0), trafficPoliceApp(tpa) {}

    void showMenu() override;
    void viewProfile() const override;
    void bookAppointment();
    void viewChallan() const;
    void payChallan();
    void generateInvoice();
    void processLicense();
    const string& getLicenseNumber() const 
    {
        return licenseNumber;
    }

    // Public method to set hasChallan
    void setChallanStatus(bool status) 
    {
        hasChallan = status;
    }

    // New method to allow users to input appointment time
    void chooseAppointmentTime();

    // New method to get appointment time
    time_t getAppointmentTime() const 
    {
        return appointmentTime;
    }
};

class TrafficPoliceApp 
{
private:
    vector<Public> users;
    const string usersFilename = "user_data.csv";

public:
    TrafficPoliceApp();
    ~TrafficPoliceApp();

    void run();
    void displayMainMenu();
    void displayTrafficInstructions();
    void displayViolationTable();
    void displayLicenseRequirements();
    void displayLicenseRenewalRequirements();
    void userLogin();
    void createUser();
    void handleChallan(Public& user, TrafficViolation::ViolationType violationType);
    time_t allocateAppointmentSlot(const tm& preferredTime);
    void readUsersFromCSV();
    void writeUsersToCSV();
};

void Public::bookAppointment() 
{
    if (hasAppointment) 
    {
        cout << "\nYou already have an appointment.\n";
        return;
    }

    chooseAppointmentTime();

    hasAppointment = true;
}

void Public::showMenu() 
{
    int choice;
    do 
    {
        cout << "\n---- User Menu ----\n";
        cout << "1. Book Appointment\n";
        cout << "2. View Challan\n";
        cout << "3. Pay Challan\n";
        cout << "4. Generate Invoice\n";
        cout << "5. Process License\n";
        cout << "6. View Profile\n";
        cout << "7. Logout\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) 
        {
        case 1:
            bookAppointment();
            break;
        case 2:
            viewChallan();
            break;
        case 3:
            payChallan();
            break;
        case 4:
            generateInvoice();
            break;
        case 5:
            processLicense();
            break;
        case 6:
            viewProfile();
            break;
        case 7:
            cout << "Logging out...\n";
            break;
        default:
            cout << "Invalid choice. Please try again.\n";
        }
    } while (choice != 7);
}

void Public::viewProfile() const 
{
    cout << "\n---- User Profile ----\n";
    cout << "Username: " << getUsername() << "\n";
    cout << "License Number: " << getLicenseNumber() << "\n";
    cout << "Has Appointment: " << (hasAppointment ? "Yes" : "No") << "\n";

    // Display appointment time if available
    if (hasAppointment) 
    {
        // Format appointment time for better readability
        cout << "Appointment Time: " << put_time(localtime(&appointmentTime), "%A, %B %d, %Y %I:%M %p") << "\n";
    }
}

void Public::viewChallan() const 
{
    if (hasChallan) 
    {
        cout << "\nChallan Details:\n";
        // Display challan details
    }
    else 
    {
        cout << "\nNo challan available.\n";
    }
}

void Public::payChallan() 
{
    if (hasChallan) 
    {
        cout << "\nChallan payment successful!\n";
        trafficPoliceApp.handleChallan(*this, TrafficViolation::SPEEDING);  // For example, handle a speeding violation
    }
    else 
    {
        cout << "\nNo challan to pay.\n";
    }
}

void Public::generateInvoice() 
{
    int violationType;
    cout << "\nEnter Violation Type (1. Speeding, 2. Red Light, 3. No Seat Belt, 4. Invalid License): ";
    cin >> violationType;

    TrafficViolation::ViolationType type;

    switch (violationType) 
    {
    case 1:
        type = TrafficViolation::SPEEDING;
        break;
    case 2:
        type = TrafficViolation::RED_LIGHT;
        break;
    case 3:
        type = TrafficViolation::NO_SEAT_BELT;
        break;
    case 4:
        type = TrafficViolation::INVALID_LICENSE;
        break;
    default:
        cout << "Invalid violation type.\n";
        return;
    }

    trafficPoliceApp.handleChallan(*this, type);
}

void Public::processLicense() 
{
    cout << "\nLicense processed successfully!\n";
}

void Public::chooseAppointmentTime() 
{
    int year, month, day, hour, minute;
    cout << "\nEnter your preferred appointment time:\n";

    cout << "Year (YYYY): ";
    cin >> year;

    cout << "Month (1-12): ";
    cin >> month;

    cout << "Day (1-31): ";
    cin >> day;

    cout << "Hour (9-17): ";
    cin >> hour;

    cout << "Minute (0-59): ";
    cin >> minute;

    tm preferredTime = {};
    preferredTime.tm_year = year - 1900; // Years since 1900
    preferredTime.tm_mon = month - 1;   // Months since January
    preferredTime.tm_mday = day;
    preferredTime.tm_hour = hour;
    preferredTime.tm_min = minute;

    appointmentTime = trafficPoliceApp.allocateAppointmentSlot(preferredTime);
}

time_t TrafficPoliceApp::allocateAppointmentSlot(const tm& preferredTime) 
{
    // Calculate the preferred time in seconds since epoch
    time_t preferredTimeInSeconds = mktime(const_cast<tm*>(&preferredTime));

    // Get the current time
    time_t currentTime = time(nullptr);

    // Calculate the difference in seconds between the preferred and current time
    long timeDifference = difftime(preferredTimeInSeconds, currentTime);

    // Check if the preferred time is at least one week later
    if (timeDifference < 7 * 24 * 60 * 60) {
        cout << "Error: Preferred appointment time must be at least one week later.\n";
        return allocateAppointmentSlot(preferredTime);
    }

    return preferredTimeInSeconds;
}

void TrafficPoliceApp::handleChallan(Public& user, TrafficViolation::ViolationType violationType) 
{
    // Simulate handling challan for a user based on violation type
    cout << "\nChallan issued successfully!\n";
    double challanAmount;

    switch (violationType) 
    {
    case TrafficViolation::SPEEDING:
        challanAmount = 2000.0; // Amount in Pakistani Rupees
        break;
    case TrafficViolation::RED_LIGHT:
        challanAmount = 1500.0; // Amount in Pakistani Rupees
        break;
    case TrafficViolation::NO_SEAT_BELT:
        challanAmount = 1000.0; // Amount in Pakistani Rupees
        break;
    case TrafficViolation::INVALID_LICENSE:
        challanAmount = 2500.0; // Amount in Pakistani Rupees
        break;
    default:
        challanAmount = 0.0; // Default value
    }

    user.setChallanStatus(true); // Update hasChallan
    // Display challan information
    cout << "Challan Amount: " << challanAmount << " PKR\n";
}

void TrafficPoliceApp::run() 
{
    readUsersFromCSV();
    displayMainMenu();
    writeUsersToCSV();
}

void TrafficPoliceApp::displayMainMenu() 
{
    int choice;
    do 
    {
        cout << "\n---- Main Menu ----\n";
        cout << "1. User Login\n";
        cout << "2. Create User\n";
        cout << "3. Traffic Instructions\n";
        cout << "4. Violation Table\n";
        cout << "5. License Requirements\n";
        cout << "6. License Renewal Requirements\n";
        cout << "7. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) 
        {
        case 1:
            userLogin();
            break;
        case 2:
            createUser();
            break;
        case 3:
            displayTrafficInstructions();
            break;
        case 4:
            displayViolationTable();
            break;
        case 5:
            displayLicenseRequirements();
            break;
        case 6:
            displayLicenseRenewalRequirements();
            break;
        case 7:
            cout << "Exiting...\n";
            break;
        default:
            cout << "Invalid choice. Please try again.\n";
        }
    } while (choice != 7);
}

void TrafficPoliceApp::displayTrafficInstructions() 
{
    cout << "\n---- Traffic Instructions ----\n";
    cout << "1. Always wear your seat belt.\n";
    cout << "2. Obey speed limits.\n";
    cout << "3. Stop at red lights.\n";
}

void TrafficPoliceApp::displayViolationTable() 
{
    cout << "\n---- Traffic Violation Table ----\n";
    cout << "Violation Type               | Challan Amount (PKR)\n";
    cout << "--------------------------------------------\n";
    cout << "1. Speeding                  | 2000.0\n";
    cout << "2. Red Light Violation       | 1500.0\n";
    cout << "3. No Seat Belt              | 1000.0\n";
    cout << "4. Invalid License           | 2500.0\n";
}

void TrafficPoliceApp::displayLicenseRequirements() 
{
    cout << "\n---- License Requirements ----\n";
    cout << "1. Age 18 or older.\n";
    cout << "2. Pass written and driving tests.\n";
    cout << "3. Provide necessary documents.\n";
}

void TrafficPoliceApp::displayLicenseRenewalRequirements() 
{
    cout << "\n---- License Renewal Requirements ----\n";
    cout << "1. Renew every 5 years.\n";
    cout << "2. Pass vision and knowledge tests.\n";
    cout << "3. Pay renewal fee.\n";
}

void TrafficPoliceApp::userLogin() 
{
    string enteredUsername, enteredPassword;

    cout << "\nEnter your username: ";
    cin >> enteredUsername;

    cout << "Enter your password: ";
    cin >> enteredPassword;

    bool isAuthenticated = false;
    for (Public& user : users) 
    {
        if (user.getUsername() == enteredUsername && user.getPassword() == enteredPassword) 
        {
            isAuthenticated = true;
            user.showMenu();
            break;
        }
    }

    if (!isAuthenticated) 
    {
        cout << "\nInvalid username or password. Please try again.\n";
    }
}

void TrafficPoliceApp::createUser() 
{
    string newUsername, newPassword, licenseNumber;

    cout << "\nEnter a new username: ";
    cin >> newUsername;

    for (const Public& user : users) 
    {
        if (user.getUsername() == newUsername) 
        {
            cout << "\nUsername already exists. Please choose a different username.\n";
            return;
        }
    }

    cout << "Enter a new password: ";
    cin >> newPassword;

    cout << "Enter the license number: ";
    cin >> licenseNumber;

    users.emplace_back(newUsername, newPassword, licenseNumber, *this);
    cout << "\nUser created successfully!\n";
}

void TrafficPoliceApp::readUsersFromCSV() 
{
    ifstream file(usersFilename);
    if (!file.is_open()) 
    {
        cout << "Error: Unable to open file " << usersFilename << "\n";
        return;
    }

    string line;
    while (getline(file, line)) 
    {
        stringstream ss(line);
        string username, password, licenseNumber;

        getline(ss, username, ',');
        getline(ss, password, ',');
        getline(ss, licenseNumber, ',');

        users.emplace_back(username, password, licenseNumber, *this);
    }

    file.close();
}

void TrafficPoliceApp::writeUsersToCSV() 
{
    ofstream file(usersFilename);
    if (!file.is_open()) 
    {
        cout << "Error: Unable to open file " << usersFilename << " for writing\n";
        return;
    }

    for (const Public& user : users) 
    {
        file << user.getUsername() << ',' << user.getPassword() << ',' << user.getLicenseNumber() << '\n';
    }

    file.close();
}

TrafficPoliceApp::TrafficPoliceApp() 
{
    readUsersFromCSV(); // Load users from CSV
}

TrafficPoliceApp::~TrafficPoliceApp() 
{
    writeUsersToCSV(); // Save users to CSV before exiting

}

int main() 
{
    TrafficPoliceApp app;
    app.run();

    return 0;
}
