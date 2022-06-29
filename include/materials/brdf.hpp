/*
 * Created by okn-yu on 2022/06/13.
 */

/*
 * レイトレーシングの扱う対象:
 * レイトレーシングで計算を行うのは光の強さ
 * 光の強さは放射エネルギーとして定式化される
 *
 * [放射エネルギー]:
 * 波長λに光子がn_λ個ある仮定する場合に全エネルギーは (n_λ * h * c)/λ として表される
 * 上の式を全波長に関して積分したものが放射エネルギー
 * つまり各波長にn_λ個の光子が含まれる光のエネルギー
 * 結局の所光とは"特定の波長の光子の集合"であり、方向性を持つエネルギーの塊である
 *
 * [放射束]:
 * 単位時間当たりの放射エネルギー
 * 物理的には単位時間当たりにある領域を通過する光子の数として解釈できる
 * レンダリングでは放射束は計算上のもっともプリミティブな値となる
 * 実際にレイトレーシングで画像を計算する場合はRGB相当の波長の放射束を計算する
 * 計算結果をRGBの色にマッピングを行う
 * TODO:以下を今後のお実装で意識すること
 * 放射束の下限は0だが上限は無限大である
 * 一方RGBのカラーは0-255という上限が存在する
 * 同じ波動でも音の大きさには上限がないが、色には上限が存在する
 * どこかで閾値があってそれ以上は認識できないのかもしれない
 * 記号はΦ
 * 単位は[W]
 *
 * [放射束面密度]:
 * 単位面積当たりの放射束
 * つまり単位時間に単位面積を通過する光子の個数
 * 単位は[W * m^-2]
 * [放射照度]:
 * 入射してくる光に関する放射束面密度
 * 記号はE
 * 単位は[W * m^-2]
 *
 * [放射発散度]:
 * 出射する光に関する放射束面密度
 * 総放射束Φを出力している面上のある点から出力される放射束
 * 放射発散度はある点からあらゆる方向に発散される放射束を扱うので方向（微小立体角）はない
 * 記号はM
 * 単位は[W * m^-2]
 *
 * [立体角]:
 * ある領域Sを半径1の単位球面上に投影した面積の大きさのこと（平面角ラジアンの3次元版）
 * 平面角は円弧の長さだが立体角は円錐となる
 * この円錐の底面の面積が立体角に相当する
 * 定義から立体角の最大値は4πで最小値は0となる
 * 面積の大きさを0に近づけることで微小立体角が得られる
 * 微小立体角を考えると円錐の底面は点となるので、微小立体角は3次元上での方向を表す
 * （面に関しては方向は定義できないが面積0の点では方向が定義できる）
 * 立体角の単位はsr(ステラジアン)
 *
 * [参考]
 *　https://batapara.com/archives/solid-angle.html/
 *
 * [放射強度]:
 * 単位立体角あたりのの放射発散度
 * 放射輝度は放射発散度の延長として定義される
 * ある微小面積が放射する総放射束を面積と立体角で微分して得られる
 * 記号はI
 * 単位は[W * sr^-1 * m^-2]
 *
 * [放射輝度]:
 * 単位立体角あたりの"単位投影面積あたり"の放射発散度
 * 放射輝度は放射発散度の延長として定義される
 * 単位投影面積は放射束と法線とのなす角度のcos倍することで得られる
 * 単位投影面積に換算する理由は、法線方向からずれるほど光を放射する面積が減少（微小面では面積に比例して放射束を放射と過程している）して届く光が暗くなることに相当する
 * ある微小面積が放射する総放射束を面積と立体角で微分するが、微小面積を投影面積に換算するために分母をcos倍する
 * 物理的にはw_o方向に対して、注目する点から単位時間あたりに飛来した(法線とのなす角度で補正した)光子の総数
 * 観測方向から見た場合、受光面の1点がどれだけ輝いているかに相当する
 * 単位は[W *sr^-1 * m^-2]
 *
 * [放射照度と放射輝度の違い]:
 * 放射照度は放射面の面積に比例するが、放射輝度は放射面の面積に依存しない
 * つまり受光面dSに対して、放射輝度は受光面が放射面から近くても遠くても値は一定
 * 遠い場合は飛来する光子の数は減少するが立体角も減少するためその比率は常に一定となる
 * 一方放射照度は、受光面が放射面に近いほど（見た目の立体角が増えるので）値は大きくなる
 *
 * [BRDF:双方向反射率分布関数]
 * f(x, w_i, w_o)の解釈
 * 点xにw_i方向から入射した光が、w_o方向に反射される光の割合
 * 入射に関しては法線とのなす角度cosの補正がかかるが、出射にはかからない（分子は放射束で分母は放射輝度）
 * 物体表面の反射特性を表しており、物体のマットな質感や光沢はこのBRDFを用いて表現される
 * 放射輝度の放射照度に関する微分として定義される（分子が反射で分母が入射）
 * 単位は[sr^-1]
 *
 * [BRDFの分母と分子]:
 * 分母は放射照度で分子は放射輝度
 * 放射輝度の強さは反射点からの距離に依存しないが、放射照度は反射点から見た入射光の立体角の大きさに比例する
 * 入射光の強さ全体を得るには立体角に依存する放射照度を使うべきだし、反射光の強さは計算時は距離に依存するのは望ましくない
 * そのため分母が放射照度で分子が放射輝度は当然
 *
 * [BRDFの特性]:
 * w_iとw_oに関して対称（すなわち入射と出射は反転しても同じ結果）
 * 半球の立体角で積分した結果は1以下（入射以上の反射はない）
 * BRDFに放射輝度を掛けて半球の立体角で積分すると、ある点xに全方向から入射した光が、特定の方向にどの程度反射したかを計算することができる
 *
 * [BRDFの使い方]:
 * 光の反射する点xは固定
 * 法線nも固定
 * 最終的な出力のw_oは固定
 * w_oとnのなす角度θは固定
 * w_iは半球全体を考える
 *
 * すなわち"反射する点全体に入射するあらゆる角度からの光を考慮して特定の視線方向への反射する光を計算する"
 * L(w_r, x) = integral ( f(x, w_i, w_r) * L(w_i, x) * cos(theta)) dw
 *
 * 1. 視線方向にある点から反射する光を計算したい
 * 2. その点に入射するあらゆる方向からの光を考慮する必要がある
 * 3. 2.の計算には各入射光ごとにbrdfと入射光の放射輝度を積算しcos(theta)で補正をかけ合わせた結果を合算する
 *
 * cos(theta)の解釈: 視線と法線が直交していれば反射はしないため0となる
 *
 * [注意:]
 * brdfは波長（RGB）毎の反射率に相当するのでその出力はベクトル
 *
 * [注意]:
 * 物体表面に固有の色があるわけではない
 * 人間が視覚で認識する"色"は、物体表面で入射光の各波長をどの程度反射するかに依存する
 * そして入射光の反射率はBRDFにより決定される
 * そのためbase_colorの影響はBRDFに現れる
 *
 */

/*
 * そもそも光を定量化して計算する場合、"有限面積の受光面"が存在することを前提とする必要がある
 *
 *
 */

#ifndef PRACTICEPATHTRACING_BRDF_HPP
#define PRACTICEPATHTRACING_BRDF_HPP

#endif //PRACTICEPATHTRACING_BRDF_HPP
