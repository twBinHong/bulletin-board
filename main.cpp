#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <iostream>
#include <SFML/Graphics.hpp>
#include <ctime>
#include <nlohmann/json.hpp>
#include <httplib.h>

std::string Get_Data_Time_String(const std::string& Data_Time) {
	size_t Data_Time_Pos = Data_Time.find("datetime") + 11;
	size_t Data_Time_End = Data_Time.find("+08:00", Data_Time_Pos) - 8;
	std::string Data_Time_String = Data_Time.substr(Data_Time_Pos, Data_Time_End - Data_Time_Pos + 1);
	Data_Time_String.replace(10, 1, " ");
	return Data_Time_String;
}

std::string Get_Global_String(const std::string& Global_Data) {
	size_t Global_End = Global_Data.find("}");
	size_t Global_Pos = Global_Data.find("{", 2, 1);
	std::string Get_Global_String = Global_Data.substr(Global_Pos, Global_End - Global_Pos + 1);
	return Get_Global_String;
}

std::string Get_Taiwan_String(const std::string& Global_Data) {
	size_t Taiwan_Pos = Global_Data.find("Taiwan, Republic of China");
	Taiwan_Pos = Global_Data.find_last_of("{", Taiwan_Pos);
	size_t Taiwan_End = Global_Data.find("}", Taiwan_Pos) + 1;
	std::string Get_Taiwan_String = Global_Data.substr(Taiwan_Pos, Taiwan_End - Taiwan_Pos + 1);
	return Get_Taiwan_String;
}

int main() {
	// connect world time api to get local time
	httplib::SSLClient Time_Client("worldtimeapi.org");
	std::string Data_Time = Time_Client.Get("/api/timezone/Asia/Taipei")->body;
	

	// connect COVID 19 api to get global data
	httplib::SSLClient Global_Client("api.covid19api.com");
	std::string Global_Data = Global_Client.Get("/summary")->body;
	

	std::string Global_String = Get_Global_String(Global_Data);
	nlohmann::json Json_Global = nlohmann::json::parse(Global_String);
	int Global_Confirmed = Json_Global["TotalConfirmed"];
	int Global_Death = Json_Global["TotalDeaths"];
	int Global_Recovered = Json_Global["TotalRecovered"];
	std::string Global_Date_String = Json_Global["Date"];

	// get Taiwan part
	std::string Taiwan_String = Get_Taiwan_String(Global_Data);
	nlohmann::json Json_Taiwan = nlohmann::json::parse(Taiwan_String);
	int Taiwan_Confirmed = Json_Taiwan["TotalConfirmed"];
	int Taiwan_Death = Json_Taiwan["TotalDeaths"];
	int Taiwan_Recovered = Json_Taiwan["TotalRecovered"];

	
	// load font
	sf::Font font;
	if (!font.loadFromFile("arial.ttf")) {
		std::cout << "load font error!!" << std::endl;
		return EXIT_FAILURE;
	}
	// set position in GUI
	sf::String Sf_Confirmed_String(std::to_string(Global_Confirmed));
	sf::Text Global_Confirmed_Text(Sf_Confirmed_String, font);
	Global_Confirmed_Text.setPosition(200, 40);

	sf::String Sf_Death_String(std::to_string(Global_Death));
	sf::Text Global_Death_Text(Sf_Death_String, font);
	Global_Death_Text.setPosition(200, 100);

	sf::String Sf_Recovered_String(std::to_string(Global_Recovered));
	sf::Text Global_Recovered_Text(Sf_Recovered_String, font);
	Global_Recovered_Text.setPosition(200, 160);

	sf::String Data_Time_String = Get_Data_Time_String(Data_Time);
	sf::Text Data_Time_Text(Data_Time_String, font);
	Data_Time_Text.setPosition(15, 2);

	sf::String Global_Marked("Global");
	sf::Text Global_Marked_Text(Global_Marked, font);
	Global_Marked_Text.setPosition(285, 210);

	sf::String Taiwan_Confirmed_String(std::to_string(Taiwan_Confirmed));
	sf::Text Taiwan_Confirmed_Text(Taiwan_Confirmed_String, font);
	Taiwan_Confirmed_Text.setPosition(200, 40);

	sf::String Taiwan_Recovered_String(std::to_string(Taiwan_Recovered));
	sf::Text Taiwan_Recovered_Text(Taiwan_Recovered_String, font);
	Taiwan_Recovered_Text.setPosition(200, 160);

	sf::String Taiwan_Death_String(std::to_string(Taiwan_Death));
	sf::Text Taiwan_Death_Text(Taiwan_Death_String, font);
	Taiwan_Death_Text.setPosition(200, 100);

	sf::String Taiwan_Marked("Taiwan");
	sf::Text Taiwan_Marked_Text(Taiwan_Marked, font);
	Taiwan_Marked_Text.setPosition(280, 210);
	// load png file
	sf::Texture Confirmed_Texture;
	if (!Confirmed_Texture.loadFromFile("confirmed.png")) {
		std::cout << "load confirmed.png error!!!" << std::endl;
		return EXIT_FAILURE;
	}

	sf::Texture Death_Texture;
	if (!Death_Texture.loadFromFile("death.png")) {
		std::cout << "load death.png error!!!" << std::endl;
		return EXIT_FAILURE;
	}
	
	sf::Texture Recovered_Texture;
	if (!Recovered_Texture.loadFromFile("recovered.png")) {
		std::cout << "load recovered.png error!!!" << std::endl;
		return EXIT_FAILURE;
	}

	sf::Sprite Confirmed_Sprite(Confirmed_Texture);
	Confirmed_Sprite.setPosition(15, 40);
	sf::Sprite Death_Sprite(Death_Texture);
	Death_Sprite.setPosition(15, 100);
	sf::Sprite Recovered_Sprite(Recovered_Texture);
	Recovered_Sprite.setPosition(15, 160);

	bool Current_Taiwan = false;			//	default setting 
	sf::RenderWindow Window(sf::VideoMode(400, 300), L"COVID-19");
	Window.setFramerateLimit(30);			// reduce refresh rate
	sf::Clock clock;
	while (Window.isOpen()) {
		sf::Event evt;
		// detect event
		if (Window.pollEvent(evt)) {
			if (evt.type == sf::Event::Closed) {
				Window.close();
			}
			if (evt.type == sf::Event::KeyPressed) {
				if (evt.key.code == sf::Keyboard::T) {
					Current_Taiwan = true;
				}
				else {
					Current_Taiwan = false;
				}
			}
		}
		// update data automatically.
		if (clock.getElapsedTime().asSeconds() >= 55.0f) {
			Time_Client.set_keep_alive(true);
			Data_Time = Time_Client.Get("/api/timezone/Asia/Taipei")->body;
			Data_Time_String = Get_Data_Time_String(Data_Time);
			Data_Time_Text.setString(Data_Time_String);
			
			Global_Client.set_keep_alive(true);
			Global_Data = Global_Client.Get("/summary")->body;
			std::string New_Global_String = Get_Global_String(Global_Data);
			// check if there are updates.
			if (Global_String.compare(New_Global_String)!= 0) {
				Global_String = New_Global_String;
				Json_Global = nlohmann::json::parse(Global_String);
				Global_Confirmed = Json_Global["TotalConfirmed"];
				Global_Death = Json_Global["TotalDeaths"];
				Global_Recovered = Json_Global["TotalRecovered"];

				Sf_Confirmed_String = std::to_string(Global_Confirmed);
				Global_Confirmed_Text.setString(Sf_Confirmed_String);
				Sf_Death_String = std::to_string(Global_Death);
				Global_Death_Text.setString(Sf_Death_String);
				Sf_Recovered_String = std::to_string(Global_Recovered);
				Global_Recovered_Text.setString(Sf_Recovered_String);

				Taiwan_String = Get_Taiwan_String(Global_Data);
				Json_Taiwan = nlohmann::json::parse(Taiwan_String);
				Taiwan_Confirmed = Json_Taiwan["TotalConfirmed"];
				Taiwan_Death = Json_Taiwan["TotalDeaths"];
				Taiwan_Recovered = Json_Taiwan["TotalRecovered"];

				Taiwan_Confirmed_String = std::to_string(Taiwan_Confirmed);
				Taiwan_Confirmed_Text.setString(Taiwan_Confirmed_String);
				Taiwan_Recovered_String = std::to_string(Taiwan_Recovered);
				Taiwan_Recovered_Text.setString(Taiwan_Recovered_String);
				Taiwan_Death_String = std::to_string(Taiwan_Death);
				Taiwan_Death_Text.setString(Taiwan_Death_String);
			}

			Time_Client.set_keep_alive(false);
			Global_Client.set_keep_alive(false);
			clock.restart();
		}

		Window.clear();

		Window.draw(Confirmed_Sprite);
		Window.draw(Death_Sprite);
		Window.draw(Recovered_Sprite);
		Window.draw(Data_Time_Text);

		if (Current_Taiwan) {
			Window.draw(Taiwan_Confirmed_Text);
			Window.draw(Taiwan_Death_Text);
			Window.draw(Taiwan_Recovered_Text);
			Window.draw(Taiwan_Marked_Text);
		}
		else {
			Window.draw(Global_Confirmed_Text);
			Window.draw(Global_Recovered_Text);
			Window.draw(Global_Death_Text);
			Window.draw(Global_Marked_Text);
		}
		
		Window.display();
	}


	return EXIT_SUCCESS;
}