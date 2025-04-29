


static void TestStuff()
{
    // Test your stuff here
    
    
    /*
    - - - - - - - - - - - - - - - - - - - - - - Testing new backend components - - - - - - - - - - - - - - - - - - - - - -
    */
        //std::vector<std::string> midi_files;
        //midi_files = NVFileUtils::GetFilesByExtension("/home/0xsys/Desktop",".mid");
        
        //for(int i = 0; i < midi_files.size(); i++)
        //{
        //    std::cout << "From Vector: " << midi_files[i] << "\n";
        //}
    
        // NVConf::configuration test;
        // NVConf::configuration test2;
        
    /*
        test.bass_voice_count = 700;
        test.bg_B = 48;
        test.bg_G = 32;
        test.bg_R = 56;
        test.current_soundfont = "/home/someone/black/midi/Soundfont.sf2";
        test.window_w = 580;
        test.window_h = 430;
        NVConf::WriteConfig(test);
    */
    /*
        test2 = NVConf::ReadConfig();
        std::cout << test2.bass_voice_count 
                  << "\n" << test2.bg_B 
                  << "\n" << test2.bg_G 
                  << "\n" << test2.bg_R 
                  << "\n" << test2.current_soundfont 
                  << "\n" << test2.window_h 
                  << "\n" << test2.window_w 
                  << "\n";
    */
    
    // NVi::info("Some Prefix", "Log idk\n");
    // NVi::error("Error", "Something failed lol\n");
    // NVi::warn("Warn prefix", "There might be a problem\n");
    /*
    Todo:
    Finish midi and soundfont file scanning + implementing both midi and soudfont list files
    Add support for bass_fx to reduce the loudness
    Adding about info
    Trying to make an apk build of this + practical tests
    Releasing the source code
    */
    
    //NVi::CreateMediaLists();
    //    
    //NVConf::CreateSoundfontList(live_soundfont_list);
    
    /*
    std::vector<SoundfontItem> reading = NVConf::ReadSoundfontList();
    for(int i = 0; i < reading.size(); i++)
    {
        std::cout << "Reading: " << reading[i].label << " | Enabled: " << reading[i].checked << "\n";
    }
    */
        
}
