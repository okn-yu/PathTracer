/*
 * Created by okn-yu on 2022/06/13.
 */

/*
 * 放射エネルギー:
 * 波長λに光子がn_λ個ある仮定する場合に全エネルギーは (n_λ * h * c)/λ として表される
 * 上の式を全波長に関して積分したものが放射エネルギー
 * つまり各波長にn_λ個の光子が含まれる光のエネルギー
 *
 * 放射束:
 * 単位時間当たりの放射エネルギー
 * 物理的には単位時間当たりにある領域を通過する光子の数として解釈できる
 *
 * 放射束面密度:
 * 単位面積当たりの放射束
 *
 * 放射照度:
 * 入射してくる光に関する放射束面密度
 *
 * 放射発散度:
 * 出射する光に関する放射束面密度
 *
 * 放射輝度:
 * 単位立体角あたりの単位投影面積あたりの放射束
 *
 * BRDF:双方向反射率分布関数
 * f(x, w_i, w_o)の解釈
 * 点xにw_i方向から入射した光がw_o方向に反射される光の割合
 * 物体表面の反射特性を表しており、物体のマットな質感や光沢はこのBRDFを用いて表現される
 * 放射輝度の放射照度に関する微分として定義される
 *
 */

#ifndef PRACTICEPATHTRACING_BRDF_HPP
#define PRACTICEPATHTRACING_BRDF_HPP

#endif //PRACTICEPATHTRACING_BRDF_HPP
