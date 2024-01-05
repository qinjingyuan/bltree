#include "btree.h"


namespace stx{


    // generate duoxiangshi model
    template<typename T>
    bool btree::node_training(T* n){
        // sure s1 != 0, define 5 site for compute k.

        int hi,lo,ele_gap,ele_num,base_site,r;
        double up_sum=0,down_sum=0,k=0,b=0,errs=0,fa=0,fb=0;
        hi = n->slotuse-1;
        lo = 0;
        ele_gap = 32;
        ele_num = hi / ele_gap + 1;
        base_site = (hi % ele_gap) / 2;
        /* defines target sites, keys, k and b */
        std::vector<double> keys;
        std::vector<double> sites;
        std::vector<double> ka(ele_num-1);
        std::vector<double> kb(ele_num-1);

        if(hi < 64) {
            n->model_type = modelType::LINE;
            n->fk[0] = static_cast<double>(hi - lo) / static_cast<double>(n->slotkey[hi] - n->slotkey[lo]);
            n->fb[0] = lo - n->fk[0] * n->slotkey[lo];
            return true;
        }else if(hi < 128){
            int x0,x1,x2,x3,x4;
            x2 = hi >> 1;
            x1 = hi >> 2;
            x0 = 0;
            x4 = hi;
            x3 = (x2+x4) >> 1;
            sites = {
                static_cast<double>(x0), 
                static_cast<double>(x1), 
                static_cast<double>(x2), 
                static_cast<double>(x3), 
                static_cast<double>(x4)};
            for(auto &e : sites){
                keys.push_back(static_cast<double>(n->slotkey[static_cast<int>(e)]));
            }

        }else{

            for(int i=base_site;i<=hi;i+=ele_gap){
                keys.push_back(static_cast<double>(n->slotkey[i]));
                sites.push_back(static_cast<double>(i));
            }

            for(int i=0;i<ele_num-1;i++){
                ka[i] = (keys[i+1] - keys[i]) / (sites[i+1] - sites[i]);
                kb[i] = keys[i] - ka[i] * sites[i];
            }

        }

        assert(keys.size() == sites.size());
        ele_num = keys.size();
        r = ele_num-1;

        k = (keys[r] - keys[0]) / (sites[r] - sites[0]);
        b = keys[0] - k * sites[0];

        /* computing distance area */
        for(int i=0;i<r;i++){
            double up=0,down=0;
            compute_integrate(k, ka[i], b, kb[i], sites[i], sites[i+1], up, down);
            up_sum+=up; down_sum+=down;
        }

        // array sites
        int x0,x1,x2,x3,x4;
        x4 = sites.size()-1;
        x2 = x4 >> 1;
        x1 = x4 >> 2;
        x3 = (x2 + x4) >> 1;
        x0 = 0;

        double k0,k1,k2,k3,k4;
        k0 = static_cast<double>(keys[x1] - keys[x0]) / (sites[x1] - sites[x0]);
        k1 = static_cast<double>(keys[x2] - keys[x1]) / (sites[x2] - sites[x1]);
        k2 = static_cast<double>(keys[x3] - keys[x2]) / (sites[x3] - sites[x2]);
        k3 = static_cast<double>(keys[x4] - keys[x3]) / (sites[x4] - sites[x3]);

        fa = (sites[r] - sites[0]) / (keys[r] - keys[0]);
        fb = sites[0] - fa * keys[0];
        // std::cout << fa << " " << fb << "\n";
        errs = (up_sum + down_sum) / (keys[r] - keys[0]);

        if(errs < 32){
            /* liner type */
            n->model_type = modelType::LINE;
            n->fk[0] = fa;
            n->fb[0] = fb;
            // node_type_counts[0]++;
        }

        else if(errs < 96){
            if(k > k0 && k < k3){
                // 小凸 small convex
                n->model_type = modelType::LINE;
                n->fk[0] = fa;
                n->fb[0] = fb + 0.8*errs;

            }else if(k < k0 && k > k3){
                // 小凹 small concave
                n->model_type = modelType::LINE;
                n->fk[0] = fa;
                n->fb[0] = fb - 0.8*errs;
            }else{
                // small S type
                n->model_type = modelType::LINE;
                n->fk[0] = fa;
                n->fb[0] = fb;
            }
        }

        else{
            std::vector<std::vector<double>> dp(ele_num,std::vector<double>(ele_num,0));
            // dp
            for(int i=0;i<ele_num;i++){
                for(int j=i;j<ele_num;j++){
                    dp[i][j] = meaning_distance(ka, kb, keys, sites, i, j);
                }
            }
            // int keys_size = keys.size();
            // for(int i=0;i<keys_size;i++){
            //     std::cout << keys[i] << " ";
            // }
            // std::cout << "\n";


            double final_k[3] = {0};
            double final_b[3] = {0};

            if(k > k0 && k < k3){
                n->model_type = modelType::TU;
                double min = DBL_MAX;
                int sp_site[4] = {0};
                for(int i=1;i<ele_num-1;i++){
                    double sum = dp[0][i] + dp[i][ele_num-1];
                    if(sum < min){
                        sp_site[0] = 0;
                        sp_site[1] = i;
                        sp_site[2] = ele_num-1;
                        min = sum;
                    }
                    // std::cout << sum << "\n";
                }
                
                // cout_nodeinfo(n);
                for(int i=0;i<2;i++){
                    final_k[i] = (sites[sp_site[i+1]] - sites[sp_site[i]]) / (keys[sp_site[i+1]] - keys[sp_site[i]])  ;
                    // std::cout << keys[sp_site[i+1]] << "\n";
                    final_b[i] = sites[sp_site[i]] - final_k[i] * keys[sp_site[i]];

                }
                // std::cout << final_k[0] << " " << final_k[1] << " " << final_k[2] << "\n";

            }else if(k < k0 && k > k3){
                n->model_type = modelType::AO;
                double min = DBL_MAX;
                int sp_site[4] = {0};
                for(int i=1;i<ele_num-1;i++){
                    double sum = dp[0][i] + dp[i][ele_num-1];
                    if(sum < min){
                        sp_site[0] = 0;
                        sp_site[1] = i;
                        sp_site[2] = ele_num-1;
                        min = sum;
                    }
                    // std::cout << sum << "\n";
                }
                
                // cout_nodeinfo(n);
                for(int i=0;i<2;i++){
                    final_k[i] = (sites[sp_site[i+1]] - sites[sp_site[i]]) / (keys[sp_site[i+1]] - keys[sp_site[i]])  ;
                    // std::cout << keys[sp_site[i+1]] << "\n";
                    final_b[i] = sites[sp_site[i]] - final_k[i] * keys[sp_site[i]];

                }
                // std::cout << final_k[0] << " " << final_k[1] << " " << final_k[2] << "\n";

            }else if(k > k0 && k > k3){
                n->model_type = modelType::S1;
                double min = DBL_MAX;
                int sp_site[4] = {0};
                for(int i=1;i<ele_num-2;i++){
                    for(int j=i+1;j<ele_num-1;j++){
                        double sum = dp[0][i] + dp[i][j] + dp[j][ele_num-1];
                        if(sum < min){
                            sp_site[0] = 0;
                            sp_site[1] = i;
                            sp_site[2] = j;
                            sp_site[3] = ele_num-1;
                            min = sum;
                        }
                        // std::cout << sum << "\n";
                    }
                }
                
                // cout_nodeinfo(n);
                for(int i=0;i<3;i++){
                    final_k[i] = (sites[sp_site[i+1]] - sites[sp_site[i]]) / (keys[sp_site[i+1]] - keys[sp_site[i]])  ;
                    // std::cout << keys[sp_site[i+1]] << "\n";
                    final_b[i] = sites[sp_site[i]] - final_k[i] * keys[sp_site[i]];

                }
                // std::cout << final_k[0] << " " << final_k[1] << " " << final_k[2] << "\n";

            }else{
                n->model_type = modelType::S2;
                double min = DBL_MAX;
                int sp_site[4] = {0};
                for(int i=1;i<ele_num-2;i++){
                    for(int j=i+1;j<ele_num-1;j++){
                        double sum = dp[0][i] + dp[i][j] + dp[j][ele_num-1];
                        if(sum < min){
                            sp_site[0] = 0;
                            sp_site[1] = i;
                            sp_site[2] = j;
                            sp_site[3] = ele_num-1;
                            min = sum;
                        }
                        // std::cout << sum << "\n";
                    }
                }
                
                // cout_nodeinfo(n);
                for(int i=0;i<3;i++){
                    final_k[i] = (sites[sp_site[i+1]] - sites[sp_site[i]]) / (keys[sp_site[i+1]] - keys[sp_site[i]])  ;
                    // std::cout << keys[sp_site[i+1]] << "\n";
                    final_b[i] = sites[sp_site[i]] - final_k[i] * keys[sp_site[i]];

                }
                // std::cout << final_k[0] << " " << final_k[1] << " " << final_k[2] << "\n";

            }

            // std::cout << final_k[0] << " " << final_k[1] << " " << final_k[2] << "\n";
            memcpy(n->fk,final_k,sizeof(final_k));
            memcpy(n->fb,final_b,sizeof(final_b));
            // std::cout << n->fk[0] << " " << n->fk[1] << " " << n->fk[2] << "\n";
        }

        n->insert_count = 0;
        n->delete_count = 0;
        return true;
    }

}
