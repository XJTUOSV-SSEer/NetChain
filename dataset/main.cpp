#include "process_dataset.hpp"
#include <iostream>



/*
    构造数据集
*/
int main(void) {
    // std::string filename = "../gplus_combined.txt";
    // std::string target_file = "../dataset1.txt";
    // size_t seg_size = 8;
    // process_dataset::process_other_dataset(filename, target_file, seg_size);
    // process_dataset::stat_dataset(target_file);

    // std::string filename = "../paysim.csv";
    // std::string target_file = "../dataset2.txt";
    // size_t seg_size = 8;
    // process_dataset::process_paysim_dataset(filename, target_file, seg_size);
    // process_dataset::stat_dataset(target_file);

    std::string filename_user = "../loc-gowalla_edges.txt";
    std::string filename_loc = "../loc-gowalla_totalCheckins.txt";
    std::string target_file = "../dataset3.txt";
    size_t seg_size = 8;
    process_dataset::process_gowalla_dataset(filename_user, filename_loc, target_file, seg_size);
    process_dataset::stat_dataset(target_file);


    // std::string file_user_artists = "../lastfm_dataset/user_artists.dat";
    // std::string file_user_friends = "../lastfm_dataset/user_friends.dat";
    // std::string file_user_artist_tags = "../lastfm_dataset/user_taggedartists-timestamps.dat";
    // std::string target_file = "../dataset4.txt";
    // size_t seg_size = 8;
    // process_dataset::process_lastfm_dataset(file_user_artists, file_user_friends, file_user_artist_tags, target_file, seg_size);
    // process_dataset::stat_dataset(target_file);
}