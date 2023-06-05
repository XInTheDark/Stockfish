/*
  Stockfish, a UCI chess playing engine derived from Glaurung 2.1
  Copyright (C) 2004-2023 The Stockfish developers (see AUTHORS file)

  Stockfish is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Stockfish is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstring>   // For std::memset
#include <iostream>
#include <numeric>
#include <sstream>

#include "evaluate.h"
#include "misc.h"
#include "movegen.h"
#include "movepick.h"
#include "position.h"
#include "search.h"
#include "thread.h"
#include "timeman.h"
#include "tt.h"
#include "uci.h"
#include "syzygy/tbprobe.h"
#include "nnue/evaluate_nnue.h"

using namespace Stockfish;

int ha0=-4,ha1=11,ha2=-1,ha3=19,ha4=10,ha5=4,ha6=9,ha7=4,ha8=-18,ha9=15,ha10=-23,ha11=-6,ha12=28,ha13=-13,ha14=29,ha15=6,ha16=15,ha17=-19,ha18=23,ha19=-23,ha20=-28,ha21=27,ha22=1,ha23=-1,ha24=18,ha25=-2,ha26=-9,ha27=8,ha28=20,ha29=15,ha30=31,ha31=-12,ha32=7,ha33=31,ha34=-8,ha35=4,ha36=-57,ha37=23,ha38=36,ha39=12,ha40=4,ha41=18,ha42=-23,ha43=-5,ha44=8,ha45=-14,ha46=-15,ha47=24,ha48=1,ha49=12,ha50=19,ha51=2,ha52=34,ha53=12,ha54=-21,ha55=7,ha56=-7,ha57=-54,ha58=-17,ha59=19,ha60=-6,ha61=-8,ha62=-25,ha63=-7,ha64=-20,ha65=8,ha66=-7,ha67=-24,ha68=-48,ha69=-9,ha70=21,ha71=-2,ha72=-12,ha73=-10,ha74=-35,ha75=-25,ha76=-4,ha77=-4,ha78=12,ha79=-7,ha80=-51,ha81=-4,ha82=-28,ha83=-31,ha84=-7,ha85=38,ha86=-13,ha87=-9,ha88=47,ha89=9,ha90=-21,ha91=8,ha92=14,ha93=42,ha94=-6,ha95=3,ha96=20,ha97=20,ha98=-25,ha99=10,ha100=4,ha101=28,ha102=-6,ha103=12,ha104=-17,ha105=8,ha106=11,ha107=-25,ha108=26,ha109=-9,ha110=-24,ha111=2,ha112=5,ha113=9,ha114=-18,ha115=-17,ha116=-17,ha117=15,ha118=10,ha119=6,ha120=-15,ha121=-22,ha122=-16,ha123=17,ha124=12,ha125=-20,ha126=7,ha127=-21,ha128=2,ha129=-15,ha130=-1,ha131=16,ha132=-17,ha133=-6,ha134=3,ha135=4,ha136=16,ha137=-22,ha138=34,ha139=2,ha140=23,ha141=32,ha142=-13,ha143=27,ha144=-9,ha145=-12,ha146=40,ha147=13,ha148=-11,ha149=19,ha150=3,ha151=7,ha152=13,ha153=7,ha154=-4,ha155=16,ha156=7,ha157=-3,ha158=-18,ha159=8,ha160=24,ha161=1,ha162=33,ha163=15,ha164=-11,ha165=18,ha166=-2,ha167=6,ha168=-16,ha169=9,ha170=6,ha171=-2,ha172=-14,ha173=-23,ha174=-1,ha175=33,ha176=-15,ha177=-13,ha178=-37,ha179=-12,ha180=-31,ha181=-14,ha182=-25,ha183=5,ha184=-5,ha185=-3,ha186=34,ha187=-29,ha188=-1,ha189=-4,ha190=10,ha191=-16,ha192=20,ha193=-12,ha194=41,ha195=17,ha196=-12,ha197=27,ha198=8,ha199=13,ha200=12,ha201=7,ha202=6,ha203=3,ha204=-28,ha205=-21,ha206=21,ha207=-21,ha208=-5,ha209=-10,ha210=-6,ha211=-22,ha212=-1,ha213=-2,ha214=-8,ha215=-20,ha216=21,ha217=18,ha218=7,ha219=-18,ha220=27,ha221=1,ha222=-3,ha223=17,ha224=-22,ha225=3,ha226=-9,ha227=-15,ha228=-23,ha229=25,ha230=1,ha231=-14,ha232=-22,ha233=-4,ha234=-35,ha235=-1,ha236=21,ha237=-26,ha238=-2,ha239=-38,ha240=21,ha241=-29,ha242=2,ha243=2,ha244=-26,ha245=-25,ha246=25,ha247=-19,ha248=-6,ha249=3,ha250=-6,ha251=10,ha252=8,ha253=26,ha254=-3,ha255=-20,ha256=-11,ha257=8,ha258=-5,ha259=-14,ha260=-19,ha261=9,ha262=-12,ha263=5,ha264=59,ha265=-8,ha266=-2,ha267=-12,ha268=-2,ha269=-5,ha270=8,ha271=30,ha272=-22,ha273=-21,ha274=-1,ha275=27,ha276=4,ha277=61,ha278=24,ha279=-19,ha280=-19,ha281=22,ha282=22,ha283=31,ha284=-7,ha285=-3,ha286=19,ha287=36,ha288=14,ha289=25,ha290=-2,ha291=-14,ha292=2,ha293=-23,ha294=45,ha295=-5,ha296=28,ha297=7,ha298=-6,ha299=4,ha300=-2,ha301=-8,ha302=36,ha303=23,ha304=-2,ha305=17,ha306=-11,ha307=1,ha308=-34,ha309=-23,ha310=-5,ha311=16,ha312=-8,ha313=-3,ha314=-22,ha315=14,ha316=19,ha317=11,ha318=-24,ha319=-13,ha320=19,ha321=6,ha322=-14,ha323=17,ha324=13,ha325=5,ha326=-5,ha327=1,ha328=6,ha329=-33,ha330=-19,ha331=21,ha332=-23,ha333=-20,ha334=26,ha335=12,ha336=-13,ha337=-5,ha338=4,ha339=-26,ha340=13,ha341=8,ha342=-25,ha343=-6,ha344=-27,ha345=19,ha346=-10,ha347=27,ha348=-8,ha349=-8,ha350=19,ha351=-10,ha352=-3,ha353=-15,ha354=36,ha355=-5,ha356=-8,ha357=23,ha358=-10,ha359=13,ha360=28,ha361=-7,ha362=-10,ha363=2,ha364=8,ha365=-8,ha366=-27,ha367=-28,ha368=9,ha369=26,ha370=-3,ha371=-25,ha372=2,ha373=3,ha374=8,ha375=2,ha376=-13,ha377=-18,ha378=25,ha379=20,ha380=-1,ha381=-3,ha382=21,ha383=-2,ha384=-9,ha385=21,ha386=3,ha387=4,ha388=15,ha389=-7,ha390=-14,ha391=-2,ha392=-7,ha393=-8,ha394=4,ha395=-13,ha396=13,ha397=-15,ha398=-14,ha399=-12,ha400=-30,ha401=-17,ha402=-8,ha403=-5,ha404=-15,ha405=-6,ha406=9,ha407=31,ha408=-33,ha409=9,ha410=9,ha411=22,ha412=-8,ha413=-6,ha414=3,ha415=12,ha416=16,ha417=18,ha418=-12,ha419=21,ha420=-9,ha421=-11,ha422=18,ha423=24,ha424=40,ha425=2,ha426=-1,ha427=5,ha428=3,ha429=-5,ha430=10,ha431=19,ha432=-3,ha433=5,ha434=10,ha435=4,ha436=5,ha437=33,ha438=-8,ha439=15,ha440=-9,ha441=15,ha442=15,ha443=10,ha444=11,ha445=-19,ha446=12,ha447=16,ha448=6,ha449=3,ha450=-27,ha451=3,ha452=-20,ha453=8,ha454=16,ha455=5,ha456=-9,ha457=34,ha458=16,ha459=28,ha460=-11,ha461=-8,ha462=12,ha463=12,ha464=3,ha465=9,ha466=18,ha467=-11,ha468=17,ha469=2,ha470=21,ha471=14,ha472=19,ha473=3,ha474=-6,ha475=25,ha476=5,ha477=3,ha478=11,ha479=-15,ha480=-2,ha481=9,ha482=-22,ha483=-22,ha484=-9,ha485=27,ha486=14,ha487=9,ha488=8,ha489=-21,ha490=-6,ha491=17,ha492=8,ha493=23,ha494=-1,ha495=15,ha496=-5,ha497=30,ha498=6,ha499=-26,ha500=13,ha501=-6,ha502=22,ha503=-5,ha504=16,ha505=16,ha506=14,ha507=14,ha508=-34,ha509=11,ha510=-3,ha511=-18,ha512=9,ha513=13,ha514=-18,ha515=22,ha516=11,ha517=3,ha518=14,ha519=-26,ha520=-27,ha521=16,ha522=-1,ha523=-11,ha524=4,ha525=-6,ha526=-15,ha527=30,ha528=-17,ha529=16,ha530=-35,ha531=-10,ha532=-3,ha533=2,ha534=-30,ha535=-16,ha536=5,ha537=2,ha538=21,ha539=14,ha540=1,ha541=2,ha542=-17,ha543=-19,ha544=-22,ha545=19,ha546=6,ha547=9,ha548=-4,ha549=26,ha550=12,ha551=-32,ha552=-10,ha553=1,ha554=8,ha555=-27,ha556=3,ha557=9,ha558=3,ha559=29,ha560=-5,ha561=-9,ha562=-9,ha563=-11,ha564=29,ha565=12,ha566=14,ha567=7,ha568=-4,ha569=-13,ha570=-10,ha571=17,ha572=15,ha573=14,ha574=1,ha575=-18,ha576=10,ha577=5,ha578=35,ha579=1,ha580=9,ha581=-9,ha582=4,ha583=6,ha584=-24,ha585=22,ha586=26,ha587=8,ha588=-9,ha589=-17,ha590=8,ha591=-8,ha592=-35,ha593=11,ha594=-21,ha595=8,ha596=3,ha597=-5,ha598=-2,ha599=-19,ha600=12,ha601=15,ha602=8,ha603=25,ha604=19,ha605=25,ha606=-16,ha607=20,ha608=14,ha609=6,ha610=17,ha611=29,ha612=17,ha613=5,ha614=27,ha615=25,ha616=13,ha617=-18,ha618=10,ha619=29,ha620=-15,ha621=-11,ha622=13,ha623=22,ha624=11,ha625=-10,ha626=-22,ha627=3,ha628=-12,ha629=18,ha630=6,ha631=29,ha632=-4,ha633=-36,ha634=-8,ha635=4,ha636=36,ha637=18,ha638=-5,ha639=14,ha640=3,ha641=3,ha642=6,ha643=13,ha644=37,ha645=-34,ha646=4,ha647=-24,ha648=10,ha649=-8,ha650=-19,ha651=-11,ha652=-33,ha653=8,ha654=-25,ha655=-10,ha656=-3,ha657=11,ha658=22,ha659=-3,ha660=7,ha661=10,ha662=-2,ha663=25,ha664=-29,ha665=-20,ha666=-6,ha667=-19,ha668=39,ha669=3,ha670=-22,ha671=-4,ha672=-2,ha673=-1,ha674=-11,ha675=16,ha676=-12,ha677=11,ha678=-5,ha679=-26,ha680=-56,ha681=-10,ha682=-20,ha683=-15,ha684=2,ha685=8,ha686=-5,ha687=6,ha688=10,ha689=-21,ha690=-9,ha691=9,ha692=13,ha693=-3,ha694=-24,ha695=-11,ha696=22,ha697=-1,ha698=7,ha699=4,ha700=36,ha701=34,ha702=-13,ha703=-17,ha704=-27,ha705=-18,ha706=3,ha707=-7,ha708=22,ha709=-22,ha710=17,ha711=-10,ha712=20,ha713=-6,ha714=10,ha715=9,ha716=15,ha717=9,ha718=-39;

int hb0=-16,hb1=-2,hb2=-7,hb3=-9,hb4=-8,hb5=-7,hb6=-8,hb7=2,hb8=-35,hb9=-11,hb10=32,hb11=25,hb12=-6,hb13=-9,hb14=11,hb15=12,hb16=-13,hb17=-8,hb18=5,hb19=3,hb20=-5,hb21=5,hb22=-33,hb23=2,hb24=24,hb25=-25,hb26=-19,hb27=27,hb28=-45,hb29=-21,hb30=-24,hb31=2,hb32=14,hb33=-7,hb34=-3,hb35=17,hb36=-13,hb37=6,hb38=-16,hb39=3,hb40=-22,hb41=3,hb42=18,hb43=16,hb44=11,hb45=2,hb46=-20,hb47=-17,hb48=17,hb49=28,hb50=-7,hb51=-2,hb52=11,hb53=-43,hb54=14,hb55=15,hb56=-3,hb57=-27,hb58=26,hb59=-26,hb60=-12,hb61=6,hb62=-31,hb63=11,hb64=6,hb65=-29,hb66=-7,hb67=-7,hb68=-24,hb69=7,hb70=9,hb71=9,hb72=-20,hb73=-18,hb74=-2,hb75=-2,hb76=16,hb77=9,hb78=-7,hb79=3,hb80=-12,hb81=4,hb82=-20,hb83=7,hb84=1,hb85=-9,hb86=-8,hb87=-20,hb88=4,hb89=2,hb90=13,hb91=20,hb92=-21,hb93=-14,hb94=-4,hb95=-8,hb96=15,hb97=25,hb98=8,hb99=17,hb100=18,hb101=24,hb102=7,hb103=-31,hb104=11,hb105=7,hb106=-10,hb107=-13,hb108=-11,hb109=-3,hb110=15,hb111=-31,hb112=1,hb113=2,hb114=-15,hb115=-6,hb116=-17,hb117=-2,hb118=-11,hb119=-28,hb120=13,hb121=10,hb122=-7,hb123=-31,hb124=10,hb125=-23,hb126=-6,hb127=-32,hb128=4,hb129=5,hb130=6,hb131=23,hb132=16,hb133=-7,hb134=-2,hb135=8,hb136=-18,hb137=7,hb138=15,hb139=-3,hb140=26,hb141=-22,hb142=-26,hb143=-19,hb144=27,hb145=19,hb146=-10,hb147=6,hb148=-25,hb149=10,hb150=-16,hb151=19,hb152=17,hb153=-2,hb154=-7,hb155=-23,hb156=-12,hb157=-24,hb158=-42,hb159=6,hb160=11,hb161=4,hb162=-7,hb163=-20,hb164=-31,hb165=11,hb166=19,hb167=29,hb168=-29,hb169=-15,hb170=-1,hb171=-22,hb172=13,hb173=-6,hb174=1,hb175=3,hb176=12,hb177=6,hb178=10,hb179=11,hb180=15,hb181=16,hb182=-3,hb183=16,hb184=-14,hb185=-27,hb186=-8,hb187=-26,hb188=20,hb189=-9,hb190=34,hb191=-12,hb192=27,hb193=-27,hb194=10,hb195=-34,hb196=33,hb197=-23,hb198=-13,hb199=-24,hb200=20,hb201=9,hb202=-5,hb203=38,hb204=9,hb205=-25,hb206=-13,hb207=-10,hb208=-5,hb209=-30,hb210=13,hb211=-6,hb212=-15,hb213=7,hb214=-2,hb215=22,hb216=5,hb217=-19,hb218=2,hb219=-14,hb220=-39,hb221=-23,hb222=27,hb223=30,hb224=-26,hb225=1,hb226=26,hb227=1,hb228=-21,hb229=24,hb230=-20,hb231=-1,hb232=22,hb233=-16,hb234=-7,hb235=6,hb236=-14,hb237=-15,hb238=6,hb239=4,hb240=-1,hb241=-8,hb242=8,hb243=-18,hb244=8,hb245=-32,hb246=-24,hb247=31,hb248=-37,hb249=4,hb250=-2,hb251=-36,hb252=12,hb253=-3,hb254=35,hb255=-3,hb256=-22,hb257=10,hb258=-2,hb259=-7,hb260=21,hb261=3,hb262=-10,hb263=38,hb264=-2,hb265=30,hb266=2,hb267=19,hb268=3,hb269=-6,hb270=-23,hb271=-1,hb272=13,hb273=-1,hb274=-23,hb275=17,hb276=-21,hb277=19,hb278=7,hb279=4,hb280=63,hb281=-13,hb282=-7,hb283=-9,hb284=34,hb285=-4,hb286=-42,hb287=23,hb288=9,hb289=6,hb290=17,hb291=-18,hb292=5,hb293=-14,hb294=-23,hb295=-2,hb296=1,hb297=13,hb298=-3,hb299=8,hb300=12,hb301=-3,hb302=12,hb303=11,hb304=-4,hb305=9,hb306=-45,hb307=3,hb308=33,hb309=-5,hb310=-13,hb311=-2,hb312=43,hb313=-12,hb314=16,hb315=5,hb316=-12,hb317=12,hb318=5,hb319=20,hb320=-3,hb321=-3,hb322=14,hb323=-8,hb324=5,hb325=-17,hb326=12,hb327=1,hb328=1,hb329=32,hb330=2,hb331=17,hb332=10,hb333=5,hb334=6,hb335=4,hb336=9,hb337=9,hb338=11,hb339=2,hb340=-5,hb341=21,hb342=11,hb343=-5,hb344=4,hb345=-8,hb346=-20,hb347=-10,hb348=-6,hb349=7,hb350=6,hb351=-21,hb352=-7,hb353=25,hb354=2,hb355=24,hb356=-13,hb357=8,hb358=-10,hb359=29,hb360=-17,hb361=19,hb362=5,hb363=17,hb364=6,hb365=-33,hb366=-7,hb367=-2,hb368=-21,hb369=4,hb370=-3,hb371=-8,hb372=-35,hb373=-3,hb374=20,hb375=-23,hb376=-9,hb377=23,hb378=15,hb379=-11,hb380=-12,hb381=-9,hb382=-29,hb383=54,hb384=-10,hb385=-4,hb386=35,hb387=-33,hb388=16,hb389=-25,hb390=-2,hb391=-22,hb392=7,hb393=-6,hb394=25,hb395=-43,hb396=3,hb397=18,hb398=-3,hb399=19,hb400=12,hb401=3,hb402=1,hb403=13,hb404=-2,hb405=-2,hb406=-11,hb407=20,hb408=18,hb409=10,hb410=33,hb411=3,hb412=2,hb413=-5,hb414=-7,hb415=-5,hb416=21,hb417=-3,hb418=22,hb419=-1,hb420=-18,hb421=-31,hb422=27,hb423=3,hb424=-3,hb425=-39,hb426=-17,hb427=10,hb428=-7,hb429=-1,hb430=-27,hb431=-13,hb432=5,hb433=-9,hb434=-7,hb435=12,hb436=17,hb437=4,hb438=7,hb439=9,hb440=-5,hb441=-7,hb442=23,hb443=2,hb444=-25,hb445=21,hb446=-11,hb447=43,hb448=7,hb449=4,hb450=15,hb451=-23,hb452=-5,hb453=-18,hb454=19,hb455=-18,hb456=-33,hb457=16,hb458=4,hb459=16,hb460=-4,hb461=12,hb462=-11,hb463=7,hb464=-18,hb465=4,hb466=28,hb467=-17,hb468=14,hb469=2,hb470=-23,hb471=-3,hb472=-17,hb473=19,hb474=-7,hb475=-20,hb476=7,hb477=-36,hb478=10,hb479=2,hb480=-14,hb481=3,hb482=-13,hb483=-30,hb484=-14,hb485=-9,hb486=-2,hb487=-6,hb488=10,hb489=-21,hb490=-6,hb491=-33,hb492=10,hb493=-19,hb494=2,hb495=-12,hb496=38,hb497=1,hb498=8,hb499=-18,hb500=-17,hb501=8,hb502=40,hb503=5,hb504=-14,hb505=4,hb506=-14,hb507=10,hb508=32,hb509=-9,hb510=-2,hb511=-15,hb512=-9,hb513=-12,hb514=-11,hb515=12,hb516=-4,hb517=16,hb518=15,hb519=-4,hb520=7,hb521=-46,hb522=7,hb523=23,hb524=10,hb525=-17,hb526=40,hb527=12,hb528=-5,hb529=-7,hb530=8,hb531=6,hb532=20,hb533=-28,hb534=24,hb535=-3,hb536=-9,hb537=13,hb538=1,hb539=-9,hb540=-23,hb541=-28,hb542=-5,hb543=-6,hb544=-12,hb545=-21,hb546=-21,hb547=-4,hb548=-9,hb549=-28,hb550=32,hb551=-1,hb552=17,hb553=11,hb554=24,hb555=11,hb556=5,hb557=-10,hb558=27,hb559=-6,hb560=-13,hb561=18,hb562=-5,hb563=5,hb564=19,hb565=21,hb566=-5,hb567=27,hb568=-25,hb569=-31,hb570=-18,hb571=1,hb572=-29,hb573=-16,hb574=8,hb575=-23,hb576=20,hb577=-23,hb578=-8,hb579=-5,hb580=4,hb581=22,hb582=-29,hb583=-18,hb584=4,hb585=6,hb586=-13,hb587=3,hb588=5,hb589=-15,hb590=-6,hb591=23,hb592=27,hb593=6,hb594=10,hb595=-1,hb596=-8,hb597=-58,hb598=-8,hb599=-33,hb600=21,hb601=-5,hb602=-2,hb603=21,hb604=7,hb605=26,hb606=-30,hb607=-16,hb608=-16,hb609=-29,hb610=-15,hb611=21,hb612=2,hb613=-36,hb614=-12,hb615=17,hb616=-18,hb617=-22,hb618=1,hb619=-10,hb620=-6,hb621=1,hb622=-15,hb623=-2,hb624=-26,hb625=10,hb626=15,hb627=27,hb628=-12,hb629=21,hb630=19,hb631=-21,hb632=8,hb633=20,hb634=-17,hb635=16,hb636=-30,hb637=19,hb638=1,hb639=11,hb640=20,hb641=25,hb642=4,hb643=30,hb644=-10,hb645=-36,hb646=-1,hb647=-18,hb648=-12,hb649=9,hb650=12,hb651=-15,hb652=-21,hb653=-10,hb654=1,hb655=14,hb656=18,hb657=17,hb658=-10,hb659=19,hb660=4,hb661=6,hb662=12,hb663=19,hb664=10,hb665=-13,hb666=4,hb667=9,hb668=-6,hb669=20,hb670=-20,hb671=-13,hb672=-2,hb673=14,hb674=-18,hb675=-21,hb676=-20,hb677=-19,hb678=30,hb679=6,hb680=8,hb681=3,hb682=-7,hb683=17,hb684=5,hb685=-21,hb686=14,hb687=-14,hb688=-7,hb689=-3,hb690=-27,hb691=-1,hb692=1,hb693=3,hb694=13,hb695=-3,hb696=-3,hb697=-7,hb698=13,hb699=-23,hb700=-2,hb701=-15,hb702=21,hb703=13,hb704=21,hb705=-12,hb706=-10,hb707=14,hb708=19,hb709=-2,hb710=3,hb711=11,hb712=-22,hb713=-43,hb714=-10,hb715=-16,hb716=3;

int hc0=-258, hc1=-399,
	hd0=-17, hd1=83,
	he0=9435;

TUNE(ha0,ha1,ha2,ha3,ha4,ha5,ha6,ha7,ha8,ha9,ha10,ha11,ha12,ha13,ha14,ha15,ha16,ha17,ha18,ha19,ha20,ha21,ha22,ha23,ha24,ha25,ha26,ha27,ha28,ha29,ha30,ha31,ha32,ha33,ha34,ha35,ha36,ha37,ha38,ha39,ha40,ha41,ha42,ha43,ha44,ha45,ha46,ha47,ha48,ha49,ha50,ha51,ha52,ha53,ha54,ha55,ha56,ha57,ha58,ha59,ha60,ha61,ha62,ha63,ha64,ha65,ha66,ha67,ha68,ha69,ha70,ha71,ha72,ha73,ha74,ha75,ha76,ha77,ha78,ha79,ha80,ha81,ha82,ha83,ha84,ha85,ha86,ha87,ha88,ha89,ha90,ha91,ha92,ha93,ha94,ha95,ha96,ha97,ha98,ha99,ha100,ha101,ha102,ha103,ha104,ha105,ha106,ha107,ha108,ha109,ha110,ha111,ha112,ha113,ha114,ha115,ha116,ha117,ha118,ha119,ha120,ha121,ha122,ha123,ha124,ha125,ha126,ha127,ha128,ha129,ha130,ha131,ha132,ha133,ha134,ha135,ha136,ha137,ha138,ha139,ha140,ha141,ha142,ha143,ha144,ha145,ha146,ha147,ha148,ha149,ha150,ha151,ha152,ha153,ha154,ha155,ha156,ha157,ha158,ha159,ha160,ha161,ha162,ha163,ha164,ha165,ha166,ha167,ha168,ha169,ha170,ha171,ha172,ha173,ha174,ha175,ha176,ha177,ha178,ha179,ha180,ha181,ha182,ha183,ha184,ha185,ha186,ha187,ha188,ha189,ha190,ha191,ha192,ha193,ha194,ha195,ha196,ha197,ha198,ha199,ha200,ha201,ha202,ha203,ha204,ha205,ha206,ha207,ha208,ha209,ha210,ha211,ha212,ha213,ha214,ha215,ha216,ha217,ha218,ha219,ha220,ha221,ha222,ha223,ha224,ha225,ha226,ha227,ha228,ha229,ha230,ha231,ha232,ha233,ha234,ha235,ha236,ha237,ha238,ha239,ha240,ha241,ha242,ha243,ha244,ha245,ha246,ha247,ha248,ha249,ha250,ha251,ha252,ha253,ha254,ha255,ha256,ha257,ha258,ha259,ha260,ha261,ha262,ha263,ha264,ha265,ha266,ha267,ha268,ha269,ha270,ha271,ha272,ha273,ha274,ha275,ha276,ha277,ha278,ha279,ha280,ha281,ha282,ha283,ha284,ha285,ha286,ha287,ha288,ha289,ha290,ha291,ha292,ha293,ha294,ha295,ha296,ha297,ha298,ha299,ha300,ha301,ha302,ha303,ha304,ha305,ha306,ha307,ha308,ha309,ha310,ha311,ha312,ha313,ha314,ha315,ha316,ha317,ha318,ha319,ha320,ha321,ha322,ha323,ha324,ha325,ha326,ha327,ha328,ha329,ha330,ha331,ha332,ha333,ha334,ha335,ha336,ha337,ha338,ha339,ha340,ha341,ha342,ha343,ha344,ha345,ha346,ha347,ha348,ha349,ha350,ha351,ha352,ha353,ha354,ha355,ha356,ha357,ha358,ha359,ha360,ha361,ha362,ha363,ha364,ha365,ha366,ha367,ha368,ha369,ha370,ha371,ha372,ha373,ha374,ha375,ha376,ha377,ha378,ha379,ha380,ha381,ha382,ha383,ha384,ha385,ha386,ha387,ha388,ha389,ha390,ha391,ha392,ha393,ha394,ha395,ha396,ha397,ha398,ha399,ha400,ha401,ha402,ha403,ha404,ha405,ha406,ha407,ha408,ha409,ha410,ha411,ha412,ha413,ha414,ha415,ha416,ha417,ha418,ha419,ha420,ha421,ha422,ha423,ha424,ha425,ha426,ha427,ha428,ha429,ha430,ha431,ha432,ha433,ha434,ha435,ha436,ha437,ha438,ha439,ha440,ha441,ha442,ha443,ha444,ha445,ha446,ha447,ha448,ha449,ha450,ha451,ha452,ha453,ha454,ha455,ha456,ha457,ha458,ha459,ha460,ha461,ha462,ha463,ha464,ha465,ha466,ha467,ha468,ha469,ha470,ha471,ha472,ha473,ha474,ha475,ha476,ha477,ha478,ha479,ha480,ha481,ha482,ha483,ha484,ha485,ha486,ha487,ha488,ha489,ha490,ha491,ha492,ha493,ha494,ha495,ha496,ha497,ha498,ha499,ha500,ha501,ha502,ha503,ha504,ha505,ha506,ha507,ha508,ha509,ha510,ha511,ha512,ha513,ha514,ha515,ha516,ha517,ha518,ha519,ha520,ha521,ha522,ha523,ha524,ha525,ha526,ha527,ha528,ha529,ha530,ha531,ha532,ha533,ha534,ha535,ha536,ha537,ha538,ha539,ha540,ha541,ha542,ha543,ha544,ha545,ha546,ha547,ha548,ha549,ha550,ha551,ha552,ha553,ha554,ha555,ha556,ha557,ha558,ha559,ha560,ha561,ha562,ha563,ha564,ha565,ha566,ha567,ha568,ha569,ha570,ha571,ha572,ha573,ha574,ha575,ha576,ha577,ha578,ha579,ha580,ha581,ha582,ha583,ha584,ha585,ha586,ha587,ha588,ha589,ha590,ha591,ha592,ha593,ha594,ha595,ha596,ha597,ha598,ha599,ha600,ha601,ha602,ha603,ha604,ha605,ha606,ha607,ha608,ha609,ha610,ha611,ha612,ha613,ha614,ha615,ha616,ha617,ha618,ha619,ha620,ha621,ha622,ha623,ha624,ha625,ha626,ha627,ha628,ha629,ha630,ha631,ha632,ha633,ha634,ha635,ha636,ha637,ha638,ha639,ha640,ha641,ha642,ha643,ha644,ha645,ha646,ha647,ha648,ha649,ha650,ha651,ha652,ha653,ha654,ha655,ha656,ha657,ha658,ha659,ha660,ha661,ha662,ha663,ha664,ha665,ha666,ha667,ha668,ha669,ha670,ha671,ha672,ha673,ha674,ha675,ha676,ha677,ha678,ha679,ha680,ha681,ha682,ha683,ha684,ha685,ha686,ha687,ha688,ha689,ha690,ha691,ha692,ha693,ha694,ha695,ha696,ha697,ha698,ha699,ha700,ha701,ha702,ha703,ha704,ha705,ha706,ha707,ha708,ha709,ha710,ha711,ha712,ha713,ha714,ha715,ha716,ha717,ha718);
TUNE(hb0,hb1,hb2,hb3,hb4,hb5,hb6,hb7,hb8,hb9,hb10,hb11,hb12,hb13,hb14,hb15,hb16,hb17,hb18,hb19,hb20,hb21,hb22,hb23,hb24,hb25,hb26,hb27,hb28,hb29,hb30,hb31,hb32,hb33,hb34,hb35,hb36,hb37,hb38,hb39,hb40,hb41,hb42,hb43,hb44,hb45,hb46,hb47,hb48,hb49,hb50,hb51,hb52,hb53,hb54,hb55,hb56,hb57,hb58,hb59,hb60,hb61,hb62,hb63,hb64,hb65,hb66,hb67,hb68,hb69,hb70,hb71,hb72,hb73,hb74,hb75,hb76,hb77,hb78,hb79,hb80,hb81,hb82,hb83,hb84,hb85,hb86,hb87,hb88,hb89,hb90,hb91,hb92,hb93,hb94,hb95,hb96,hb97,hb98,hb99,hb100,hb101,hb102,hb103,hb104,hb105,hb106,hb107,hb108,hb109,hb110,hb111,hb112,hb113,hb114,hb115,hb116,hb117,hb118,hb119,hb120,hb121,hb122,hb123,hb124,hb125,hb126,hb127,hb128,hb129,hb130,hb131,hb132,hb133,hb134,hb135,hb136,hb137,hb138,hb139,hb140,hb141,hb142,hb143,hb144,hb145,hb146,hb147,hb148,hb149,hb150,hb151,hb152,hb153,hb154,hb155,hb156,hb157,hb158,hb159,hb160,hb161,hb162,hb163,hb164,hb165,hb166,hb167,hb168,hb169,hb170,hb171,hb172,hb173,hb174,hb175,hb176,hb177,hb178,hb179,hb180,hb181,hb182,hb183,hb184,hb185,hb186,hb187,hb188,hb189,hb190,hb191,hb192,hb193,hb194,hb195,hb196,hb197,hb198,hb199,hb200,hb201,hb202,hb203,hb204,hb205,hb206,hb207,hb208,hb209,hb210,hb211,hb212,hb213,hb214,hb215,hb216,hb217,hb218,hb219,hb220,hb221,hb222,hb223,hb224,hb225,hb226,hb227,hb228,hb229,hb230,hb231,hb232,hb233,hb234,hb235,hb236,hb237,hb238,hb239,hb240,hb241,hb242,hb243,hb244,hb245,hb246,hb247,hb248,hb249,hb250,hb251,hb252,hb253,hb254,hb255,hb256,hb257,hb258,hb259,hb260,hb261,hb262,hb263,hb264,hb265,hb266,hb267,hb268,hb269,hb270,hb271,hb272,hb273,hb274,hb275,hb276,hb277,hb278,hb279,hb280,hb281,hb282,hb283,hb284,hb285,hb286,hb287,hb288,hb289,hb290,hb291,hb292,hb293,hb294,hb295,hb296,hb297,hb298,hb299,hb300,hb301,hb302,hb303,hb304,hb305,hb306,hb307,hb308,hb309,hb310,hb311,hb312,hb313,hb314,hb315,hb316,hb317,hb318,hb319,hb320,hb321,hb322,hb323,hb324,hb325,hb326,hb327,hb328,hb329,hb330,hb331,hb332,hb333,hb334,hb335,hb336,hb337,hb338,hb339,hb340,hb341,hb342,hb343,hb344,hb345,hb346,hb347,hb348,hb349,hb350,hb351,hb352,hb353,hb354,hb355,hb356,hb357,hb358,hb359,hb360,hb361,hb362,hb363,hb364,hb365,hb366,hb367,hb368,hb369,hb370,hb371,hb372,hb373,hb374,hb375,hb376,hb377,hb378,hb379,hb380,hb381,hb382,hb383,hb384,hb385,hb386,hb387,hb388,hb389,hb390,hb391,hb392,hb393,hb394,hb395,hb396,hb397,hb398,hb399,hb400,hb401,hb402,hb403,hb404,hb405,hb406,hb407,hb408,hb409,hb410,hb411,hb412,hb413,hb414,hb415,hb416,hb417,hb418,hb419,hb420,hb421,hb422,hb423,hb424,hb425,hb426,hb427,hb428,hb429,hb430,hb431,hb432,hb433,hb434,hb435,hb436,hb437,hb438,hb439,hb440,hb441,hb442,hb443,hb444,hb445,hb446,hb447,hb448,hb449,hb450,hb451,hb452,hb453,hb454,hb455,hb456,hb457,hb458,hb459,hb460,hb461,hb462,hb463,hb464,hb465,hb466,hb467,hb468,hb469,hb470,hb471,hb472,hb473,hb474,hb475,hb476,hb477,hb478,hb479,hb480,hb481,hb482,hb483,hb484,hb485,hb486,hb487,hb488,hb489,hb490,hb491,hb492,hb493,hb494,hb495,hb496,hb497,hb498,hb499,hb500,hb501,hb502,hb503,hb504,hb505,hb506,hb507,hb508,hb509,hb510,hb511,hb512,hb513,hb514,hb515,hb516,hb517,hb518,hb519,hb520,hb521,hb522,hb523,hb524,hb525,hb526,hb527,hb528,hb529,hb530,hb531,hb532,hb533,hb534,hb535,hb536,hb537,hb538,hb539,hb540,hb541,hb542,hb543,hb544,hb545,hb546,hb547,hb548,hb549,hb550,hb551,hb552,hb553,hb554,hb555,hb556,hb557,hb558,hb559,hb560,hb561,hb562,hb563,hb564,hb565,hb566,hb567,hb568,hb569,hb570,hb571,hb572,hb573,hb574,hb575,hb576,hb577,hb578,hb579,hb580,hb581,hb582,hb583,hb584,hb585,hb586,hb587,hb588,hb589,hb590,hb591,hb592,hb593,hb594,hb595,hb596,hb597,hb598,hb599,hb600,hb601,hb602,hb603,hb604,hb605,hb606,hb607,hb608,hb609,hb610,hb611,hb612,hb613,hb614,hb615,hb616,hb617,hb618,hb619,hb620,hb621,hb622,hb623,hb624,hb625,hb626,hb627,hb628,hb629,hb630,hb631,hb632,hb633,hb634,hb635,hb636,hb637,hb638,hb639,hb640,hb641,hb642,hb643,hb644,hb645,hb646,hb647,hb648,hb649,hb650,hb651,hb652,hb653,hb654,hb655,hb656,hb657,hb658,hb659,hb660,hb661,hb662,hb663,hb664,hb665,hb666,hb667,hb668,hb669,hb670,hb671,hb672,hb673,hb674,hb675,hb676,hb677,hb678,hb679,hb680,hb681,hb682,hb683,hb684,hb685,hb686,hb687,hb688,hb689,hb690,hb691,hb692,hb693,hb694,hb695,hb696,hb697,hb698,hb699,hb700,hb701,hb702,hb703,hb704,hb705,hb706,hb707,hb708,hb709,hb710,hb711,hb712,hb713,hb714,hb715,hb716);
TUNE(hc0, hc1, hd0, hd1, he0);

// Net weights and biases of a small neural network for time management
int nw[2][736] =
{
// Hidden layer weights
{ha0,ha1,ha2,ha3,ha4,ha5,ha6,ha7,ha8,ha9,ha10,ha11,ha12,ha13,ha14,ha15,ha16,ha17,ha18,ha19,ha20,ha21,ha22,ha23,ha24,ha25,ha26,ha27,ha28,ha29,ha30,ha31,ha32,ha33,ha34,ha35,ha36,ha37,ha38,ha39,ha40,ha41,ha42,ha43,ha44,ha45,ha46,ha47,ha48,ha49,ha50,ha51,ha52,ha53,ha54,ha55,ha56,ha57,ha58,ha59,ha60,ha61,ha62,ha63,ha64,ha65,ha66,ha67,ha68,ha69,ha70,ha71,ha72,ha73,ha74,ha75,ha76,ha77,ha78,ha79,ha80,ha81,ha82,ha83,ha84,ha85,ha86,ha87,ha88,ha89,ha90,ha91,ha92,ha93,ha94,ha95,ha96,ha97,ha98,ha99,ha100,ha101,ha102,ha103,ha104,ha105,ha106,ha107,ha108,ha109,ha110,ha111,ha112,ha113,ha114,ha115,ha116,ha117,ha118,ha119,ha120,ha121,ha122,ha123,ha124,ha125,ha126,ha127,ha128,ha129,ha130,ha131,ha132,ha133,ha134,ha135,ha136,ha137,ha138,ha139,ha140,ha141,ha142,ha143,ha144,ha145,ha146,ha147,ha148,ha149,ha150,ha151,ha152,ha153,ha154,ha155,ha156,ha157,ha158,ha159,ha160,ha161,ha162,ha163,ha164,ha165,ha166,ha167,ha168,ha169,ha170,ha171,ha172,ha173,ha174,ha175,ha176,ha177,ha178,ha179,ha180,ha181,ha182,ha183,ha184,ha185,ha186,ha187,ha188,ha189,ha190,ha191,ha192,ha193,ha194,ha195,ha196,ha197,ha198,ha199,ha200,ha201,ha202,ha203,ha204,ha205,ha206,ha207,ha208,ha209,ha210,ha211,ha212,ha213,ha214,ha215,ha216,ha217,ha218,ha219,ha220,ha221,ha222,ha223,ha224,ha225,ha226,ha227,ha228,ha229,ha230,ha231,ha232,ha233,ha234,ha235,ha236,ha237,ha238,ha239,ha240,ha241,ha242,ha243,ha244,ha245,ha246,ha247,ha248,ha249,ha250,ha251,ha252,ha253,ha254,ha255,ha256,ha257,ha258,ha259,ha260,ha261,ha262,ha263,ha264,ha265,ha266,ha267,ha268,ha269,ha270,ha271,ha272,ha273,ha274,ha275,ha276,ha277,ha278,ha279,ha280,ha281,ha282,ha283,ha284,ha285,ha286,ha287,ha288,ha289,ha290,ha291,ha292,ha293,ha294,ha295,ha296,ha297,ha298,ha299,ha300,ha301,ha302,ha303,ha304,ha305,ha306,ha307,ha308,ha309,ha310,ha311,ha312,ha313,ha314,ha315,ha316,ha317,ha318,ha319,ha320,ha321,ha322,ha323,ha324,ha325,ha326,ha327,ha328,ha329,ha330,ha331,ha332,ha333,ha334,ha335,ha336,ha337,ha338,ha339,ha340,ha341,ha342,ha343,ha344,ha345,ha346,ha347,ha348,ha349,ha350,ha351,ha352,ha353,ha354,ha355,ha356,ha357,ha358,ha359,ha360,ha361,ha362,ha363,ha364,ha365,ha366,ha367,ha368,ha369,ha370,ha371,ha372,ha373,ha374,ha375,ha376,ha377,ha378,ha379,ha380,ha381,ha382,ha383,ha384,ha385,ha386,ha387,ha388,ha389,ha390,ha391,ha392,ha393,ha394,ha395,ha396,ha397,ha398,ha399,ha400,ha401,ha402,ha403,ha404,ha405,ha406,ha407,ha408,ha409,ha410,ha411,ha412,ha413,ha414,ha415,ha416,ha417,ha418,ha419,ha420,ha421,ha422,ha423,ha424,ha425,ha426,ha427,ha428,ha429,ha430,ha431,ha432,ha433,ha434,ha435,ha436,ha437,ha438,ha439,ha440,ha441,ha442,ha443,ha444,ha445,ha446,ha447,ha448,ha449,ha450,ha451,ha452,ha453,ha454,ha455,ha456,ha457,ha458,ha459,ha460,ha461,ha462,ha463,ha464,ha465,ha466,ha467,ha468,ha469,ha470,ha471,ha472,ha473,ha474,ha475,ha476,ha477,ha478,ha479,ha480,ha481,ha482,ha483,ha484,ha485,ha486,ha487,ha488,ha489,ha490,ha491,ha492,ha493,ha494,ha495,ha496,ha497,ha498,ha499,ha500,ha501,ha502,ha503,ha504,ha505,ha506,ha507,ha508,ha509,ha510,ha511,ha512,ha513,ha514,ha515,ha516,ha517,ha518,ha519,ha520,ha521,ha522,ha523,ha524,ha525,ha526,ha527,ha528,ha529,ha530,ha531,ha532,ha533,ha534,ha535,ha536,ha537,ha538,ha539,ha540,ha541,ha542,ha543,ha544,ha545,ha546,ha547,ha548,ha549,ha550,ha551,ha552,ha553,ha554,ha555,ha556,ha557,ha558,ha559,ha560,ha561,ha562,ha563,ha564,ha565,ha566,ha567,ha568,ha569,ha570,ha571,ha572,ha573,ha574,ha575,ha576,ha577,ha578,ha579,ha580,ha581,ha582,ha583,ha584,ha585,ha586,ha587,ha588,ha589,ha590,ha591,ha592,ha593,ha594,ha595,ha596,ha597,ha598,ha599,ha600,ha601,ha602,ha603,ha604,ha605,ha606,ha607,ha608,ha609,ha610,ha611,ha612,ha613,ha614,ha615,ha616,ha617,ha618,ha619,ha620,ha621,ha622,ha623,ha624,ha625,ha626,ha627,ha628,ha629,ha630,ha631,ha632,ha633,ha634,ha635,ha636,ha637,ha638,ha639,ha640,ha641,ha642,ha643,ha644,ha645,ha646,ha647,ha648,ha649,ha650,ha651,ha652,ha653,ha654,ha655,ha656,ha657,ha658,ha659,ha660,ha661,ha662,ha663,ha664,ha665,ha666,ha667,ha668,ha669,ha670,ha671,ha672,ha673,ha674,ha675,ha676,ha677,ha678,ha679,ha680,ha681,ha682,ha683,ha684,ha685,ha686,ha687,ha688,ha689,ha690,ha691,ha692,ha693,ha694,ha695,ha696,ha697,ha698,ha699,ha700,ha701,ha702,ha703,ha704,ha705,ha706,ha707,ha708,ha709,ha710,ha711,ha712,ha713,ha714,ha715,ha716,ha717,ha718},

{hb0,hb1,hb2,hb3,hb4,hb5,hb6,hb7,hb8,hb9,hb10,hb11,hb12,hb13,hb14,hb15,hb16,hb17,hb18,hb19,hb20,hb21,hb22,hb23,hb24,hb25,hb26,hb27,hb28,hb29,hb30,hb31,hb32,hb33,hb34,hb35,hb36,hb37,hb38,hb39,hb40,hb41,hb42,hb43,hb44,hb45,hb46,hb47,hb48,hb49,hb50,hb51,hb52,hb53,hb54,hb55,hb56,hb57,hb58,hb59,hb60,hb61,hb62,hb63,hb64,hb65,hb66,hb67,hb68,hb69,hb70,hb71,hb72,hb73,hb74,hb75,hb76,hb77,hb78,hb79,hb80,hb81,hb82,hb83,hb84,hb85,hb86,hb87,hb88,hb89,hb90,hb91,hb92,hb93,hb94,hb95,hb96,hb97,hb98,hb99,hb100,hb101,hb102,hb103,hb104,hb105,hb106,hb107,hb108,hb109,hb110,hb111,hb112,hb113,hb114,hb115,hb116,hb117,hb118,hb119,hb120,hb121,hb122,hb123,hb124,hb125,hb126,hb127,hb128,hb129,hb130,hb131,hb132,hb133,hb134,hb135,hb136,hb137,hb138,hb139,hb140,hb141,hb142,hb143,hb144,hb145,hb146,hb147,hb148,hb149,hb150,hb151,hb152,hb153,hb154,hb155,hb156,hb157,hb158,hb159,hb160,hb161,hb162,hb163,hb164,hb165,hb166,hb167,hb168,hb169,hb170,hb171,hb172,hb173,hb174,hb175,hb176,hb177,hb178,hb179,hb180,hb181,hb182,hb183,hb184,hb185,hb186,hb187,hb188,hb189,hb190,hb191,hb192,hb193,hb194,hb195,hb196,hb197,hb198,hb199,hb200,hb201,hb202,hb203,hb204,hb205,hb206,hb207,hb208,hb209,hb210,hb211,hb212,hb213,hb214,hb215,hb216,hb217,hb218,hb219,hb220,hb221,hb222,hb223,hb224,hb225,hb226,hb227,hb228,hb229,hb230,hb231,hb232,hb233,hb234,hb235,hb236,hb237,hb238,hb239,hb240,hb241,hb242,hb243,hb244,hb245,hb246,hb247,hb248,hb249,hb250,hb251,hb252,hb253,hb254,hb255,hb256,hb257,hb258,hb259,hb260,hb261,hb262,hb263,hb264,hb265,hb266,hb267,hb268,hb269,hb270,hb271,hb272,hb273,hb274,hb275,hb276,hb277,hb278,hb279,hb280,hb281,hb282,hb283,hb284,hb285,hb286,hb287,hb288,hb289,hb290,hb291,hb292,hb293,hb294,hb295,hb296,hb297,hb298,hb299,hb300,hb301,hb302,hb303,hb304,hb305,hb306,hb307,hb308,hb309,hb310,hb311,hb312,hb313,hb314,hb315,hb316,hb317,hb318,hb319,hb320,hb321,hb322,hb323,hb324,hb325,hb326,hb327,hb328,hb329,hb330,hb331,hb332,hb333,hb334,hb335,hb336,hb337,hb338,hb339,hb340,hb341,hb342,hb343,hb344,hb345,hb346,hb347,hb348,hb349,hb350,hb351,hb352,hb353,hb354,hb355,hb356,hb357,hb358,hb359,hb360,hb361,hb362,hb363,hb364,hb365,hb366,hb367,hb368,hb369,hb370,hb371,hb372,hb373,hb374,hb375,hb376,hb377,hb378,hb379,hb380,hb381,hb382,hb383,hb384,hb385,hb386,hb387,hb388,hb389,hb390,hb391,hb392,hb393,hb394,hb395,hb396,hb397,hb398,hb399,hb400,hb401,hb402,hb403,hb404,hb405,hb406,hb407,hb408,hb409,hb410,hb411,hb412,hb413,hb414,hb415,hb416,hb417,hb418,hb419,hb420,hb421,hb422,hb423,hb424,hb425,hb426,hb427,hb428,hb429,hb430,hb431,hb432,hb433,hb434,hb435,hb436,hb437,hb438,hb439,hb440,hb441,hb442,hb443,hb444,hb445,hb446,hb447,hb448,hb449,hb450,hb451,hb452,hb453,hb454,hb455,hb456,hb457,hb458,hb459,hb460,hb461,hb462,hb463,hb464,hb465,hb466,hb467,hb468,hb469,hb470,hb471,hb472,hb473,hb474,hb475,hb476,hb477,hb478,hb479,hb480,hb481,hb482,hb483,hb484,hb485,hb486,hb487,hb488,hb489,hb490,hb491,hb492,hb493,hb494,hb495,hb496,hb497,hb498,hb499,hb500,hb501,hb502,hb503,hb504,hb505,hb506,hb507,hb508,hb509,hb510,hb511,hb512,hb513,hb514,hb515,hb516,hb517,hb518,hb519,hb520,hb521,hb522,hb523,hb524,hb525,hb526,hb527,hb528,hb529,hb530,hb531,hb532,hb533,hb534,hb535,hb536,hb537,hb538,hb539,hb540,hb541,hb542,hb543,hb544,hb545,hb546,hb547,hb548,hb549,hb550,hb551,hb552,hb553,hb554,hb555,hb556,hb557,hb558,hb559,hb560,hb561,hb562,hb563,hb564,hb565,hb566,hb567,hb568,hb569,hb570,hb571,hb572,hb573,hb574,hb575,hb576,hb577,hb578,hb579,hb580,hb581,hb582,hb583,hb584,hb585,hb586,hb587,hb588,hb589,hb590,hb591,hb592,hb593,hb594,hb595,hb596,hb597,hb598,hb599,hb600,hb601,hb602,hb603,hb604,hb605,hb606,hb607,hb608,hb609,hb610,hb611,hb612,hb613,hb614,hb615,hb616,hb617,hb618,hb619,hb620,hb621,hb622,hb623,hb624,hb625,hb626,hb627,hb628,hb629,hb630,hb631,hb632,hb633,hb634,hb635,hb636,hb637,hb638,hb639,hb640,hb641,hb642,hb643,hb644,hb645,hb646,hb647,hb648,hb649,hb650,hb651,hb652,hb653,hb654,hb655,hb656,hb657,hb658,hb659,hb660,hb661,hb662,hb663,hb664,hb665,hb666,hb667,hb668,hb669,hb670,hb671,hb672,hb673,hb674,hb675,hb676,hb677,hb678,hb679,hb680,hb681,hb682,hb683,hb684,hb685,hb686,hb687,hb688,hb689,hb690,hb691,hb692,hb693,hb694,hb695,hb696,hb697,hb698,hb699,hb700,hb701,hb702,hb703,hb704,hb705,hb706,hb707,hb708,hb709,hb710,hb711,hb712,hb713,hb714,hb715,hb716}
};
// Hidden layer biases
int nb[2] = {hc0,hc1};
// Output layer weights
int nwo[2] = {hd0,hd1};
// Output layer bias
double nbo = he0 / 10000.0;


namespace Stockfish {

namespace Search {

  LimitsType Limits;
}

namespace Tablebases {

  int Cardinality;
  bool RootInTB;
  bool UseRule50;
  Depth ProbeDepth;
}

namespace TB = Tablebases;

using std::string;
using Eval::evaluate;
using namespace Search;

namespace {

  // Different node types, used as a template parameter
  enum NodeType { NonPV, PV, Root };

  // Futility margin
  Value futility_margin(Depth d, bool improving) {
    return Value(140 * (d - improving));
  }

  // Reductions lookup table, initialized at startup
  int Reductions[MAX_MOVES]; // [depth or moveNumber]

  Depth reduction(bool i, Depth d, int mn, Value delta, Value rootDelta) {
    int r = Reductions[d] * Reductions[mn];
    return (r + 1372 - int(delta) * 1073 / int(rootDelta)) / 1024 + (!i && r > 936);
  }

  constexpr int futility_move_count(bool improving, Depth depth) {
    return improving ? (3 + depth * depth)
                     : (3 + depth * depth) / 2;
  }

  // History and stats update bonus, based on depth
  int stat_bonus(Depth d) {
    return std::min(336 * d - 547, 1561);
  }

  // Add a small random component to draw evaluations to avoid 3-fold blindness
  Value value_draw(const Thread* thisThread) {
    return VALUE_DRAW - 1 + Value(thisThread->nodes & 0x2);
  }

  // Skill structure is used to implement strength limit. If we have an uci_elo then
  // we convert it to a suitable fractional skill level using anchoring to CCRL Elo
  // (goldfish 1.13 = 2000) and a fit through Ordo derived Elo for match (TC 60+0.6)
  // results spanning a wide range of k values.
  struct Skill {
    Skill(int skill_level, int uci_elo) {
        if (uci_elo)
        {
            double e = double(uci_elo - 1320) / (3190 - 1320);
            level = std::clamp((((37.2473 * e - 40.8525) * e + 22.2943) * e - 0.311438), 0.0, 19.0);
        }
        else
            level = double(skill_level);
    }
    bool enabled() const { return level < 20.0; }
    bool time_to_pick(Depth depth) const { return depth == 1 + int(level); }
    Move pick_best(size_t multiPV);

    double level;
    Move best = MOVE_NONE;
  };

  template <NodeType nodeType>
  Value search(Position& pos, Stack* ss, Value alpha, Value beta, Depth depth, bool cutNode);

  template <NodeType nodeType>
  Value qsearch(Position& pos, Stack* ss, Value alpha, Value beta, Depth depth = 0);

  Value value_to_tt(Value v, int ply);
  Value value_from_tt(Value v, int ply, int r50c);
  void update_pv(Move* pv, Move move, const Move* childPv);
  void update_continuation_histories(Stack* ss, Piece pc, Square to, int bonus);
  void update_quiet_stats(const Position& pos, Stack* ss, Move move, int bonus);
  void update_all_stats(const Position& pos, Stack* ss, Move bestMove, Value bestValue, Value beta, Square prevSq,
                        Move* quietsSearched, int quietCount, Move* capturesSearched, int captureCount, Depth depth);

  // perft() is our utility to verify move generation. All the leaf nodes up
  // to the given depth are generated and counted, and the sum is returned.
  template<bool Root>
  uint64_t perft(Position& pos, Depth depth) {

    StateInfo st;
    ASSERT_ALIGNED(&st, Eval::NNUE::CacheLineSize);

    uint64_t cnt, nodes = 0;
    const bool leaf = (depth == 2);

    for (const auto& m : MoveList<LEGAL>(pos))
    {
        if (Root && depth <= 1)
            cnt = 1, nodes++;
        else
        {
            pos.do_move(m, st);
            cnt = leaf ? MoveList<LEGAL>(pos).size() : perft<false>(pos, depth - 1);
            nodes += cnt;
            pos.undo_move(m);
        }
        if (Root)
            sync_cout << UCI::move(m, pos.is_chess960()) << ": " << cnt << sync_endl;
    }
    return nodes;
  }

} // namespace


/// Search::init() is called at startup to initialize various lookup tables

void Search::init() {

  for (int i = 1; i < MAX_MOVES; ++i)
      Reductions[i] = int((20.57 + std::log(Threads.size()) / 2) * std::log(i));
}


/// Search::clear() resets search state to its initial value

void Search::clear() {

  Threads.main()->wait_for_search_finished();

  Time.availableNodes = 0;
  TT.clear();
  Threads.clear();
  Tablebases::init(Options["SyzygyPath"]); // Free mapped files
}


/// MainThread::search() is started when the program receives the UCI 'go'
/// command. It searches from the root position and outputs the "bestmove".

void MainThread::search() {

  if (Limits.perft)
  {
      nodes = perft<true>(rootPos, Limits.perft);
      sync_cout << "\nNodes searched: " << nodes << "\n" << sync_endl;
      return;
  }

  Color us = rootPos.side_to_move();
  Time.init(Limits, us, rootPos.game_ply());
  TT.new_search();

  Eval::NNUE::verify();

  if (rootMoves.empty())
  {
      rootMoves.emplace_back(MOVE_NONE);
      sync_cout << "info depth 0 score "
                << UCI::value(rootPos.checkers() ? -VALUE_MATE : VALUE_DRAW)
                << sync_endl;
  }
  else
  {
      Threads.start_searching(); // start non-main threads
      Thread::search();          // main thread start searching
  }

  // When we reach the maximum depth, we can arrive here without a raise of
  // Threads.stop. However, if we are pondering or in an infinite search,
  // the UCI protocol states that we shouldn't print the best move before the
  // GUI sends a "stop" or "ponderhit" command. We therefore simply wait here
  // until the GUI sends one of those commands.

  while (!Threads.stop && (ponder || Limits.infinite))
  {} // Busy wait for a stop or a ponder reset

  // Stop the threads if not already stopped (also raise the stop if
  // "ponderhit" just reset Threads.ponder).
  Threads.stop = true;

  // Wait until all threads have finished
  Threads.wait_for_search_finished();

  // When playing in 'nodes as time' mode, subtract the searched nodes from
  // the available ones before exiting.
  if (Limits.npmsec)
      Time.availableNodes += Limits.inc[us] - Threads.nodes_searched();

  Thread* bestThread = this;
  Skill skill = Skill(Options["Skill Level"], Options["UCI_LimitStrength"] ? int(Options["UCI_Elo"]) : 0);

  if (   int(Options["MultiPV"]) == 1
      && !Limits.depth
      && !skill.enabled()
      && rootMoves[0].pv[0] != MOVE_NONE)
      bestThread = Threads.get_best_thread();

  bestPreviousScore = bestThread->rootMoves[0].score;
  bestPreviousAverageScore = bestThread->rootMoves[0].averageScore;

  // Send again PV info if we have a new best thread
  if (bestThread != this)
      sync_cout << UCI::pv(bestThread->rootPos, bestThread->completedDepth) << sync_endl;

  sync_cout << "bestmove " << UCI::move(bestThread->rootMoves[0].pv[0], rootPos.is_chess960());

  if (bestThread->rootMoves[0].pv.size() > 1 || bestThread->rootMoves[0].extract_ponder_from_tt(rootPos))
      std::cout << " ponder " << UCI::move(bestThread->rootMoves[0].pv[1], rootPos.is_chess960());

  std::cout << sync_endl;
}


/// Thread::search() is the main iterative deepening loop. It calls search()
/// repeatedly with increasing depth until the allocated thinking time has been
/// consumed, the user stops the search, or the maximum search depth is reached.

void Thread::search() {

  // To allow access to (ss-7) up to (ss+2), the stack must be oversized.
  // The former is needed to allow update_continuation_histories(ss-1, ...),
  // which accesses its argument at ss-6, also near the root.
  // The latter is needed for statScore and killer initialization.
  Stack stack[MAX_PLY+10], *ss = stack+7;
  Move  pv[MAX_PLY+1];
  Value alpha, beta, delta;
  Move  lastBestMove = MOVE_NONE;
  Depth lastBestMoveDepth = 0;
  MainThread* mainThread = (this == Threads.main() ? Threads.main() : nullptr);
  double timeReduction = 1, totBestMoveChanges = 0;
  Color us = rootPos.side_to_move();
  int iterIdx = 0;

  std::memset(ss-7, 0, 10 * sizeof(Stack));
  for (int i = 7; i > 0; --i)
  {
      (ss-i)->continuationHistory = &this->continuationHistory[0][0][NO_PIECE][0]; // Use as a sentinel
      (ss-i)->staticEval = VALUE_NONE;
  }

  for (int i = 0; i <= MAX_PLY + 2; ++i)
      (ss+i)->ply = i;

  ss->pv = pv;

  bestValue = -VALUE_INFINITE;

  if (mainThread)
  {
      if (mainThread->bestPreviousScore == VALUE_INFINITE)
          for (int i = 0; i < 4; ++i)
              mainThread->iterValue[i] = VALUE_ZERO;
      else
          for (int i = 0; i < 4; ++i)
              mainThread->iterValue[i] = mainThread->bestPreviousScore;
  }

  size_t multiPV = size_t(Options["MultiPV"]);
  Skill skill(Options["Skill Level"], Options["UCI_LimitStrength"] ? int(Options["UCI_Elo"]) : 0);

  // When playing with strength handicap enable MultiPV search that we will
  // use behind the scenes to retrieve a set of possible moves.
  if (skill.enabled())
      multiPV = std::max(multiPV, (size_t)4);

  multiPV = std::min(multiPV, rootMoves.size());

  int searchAgainCounter = 0;

  // Iterative deepening loop until requested to stop or the target depth is reached
  while (   ++rootDepth < MAX_PLY
         && !Threads.stop
         && !(Limits.depth && mainThread && rootDepth > Limits.depth))
  {
      // Age out PV variability metric
      if (mainThread)
          totBestMoveChanges /= 2;

      // Save the last iteration's scores before first PV line is searched and
      // all the move scores except the (new) PV are set to -VALUE_INFINITE.
      for (RootMove& rm : rootMoves)
          rm.previousScore = rm.score;

      size_t pvFirst = 0;
      pvLast = 0;

      if (!Threads.increaseDepth)
          searchAgainCounter++;

      // MultiPV loop. We perform a full root search for each PV line
      for (pvIdx = 0; pvIdx < multiPV && !Threads.stop; ++pvIdx)
      {
          if (pvIdx == pvLast)
          {
              pvFirst = pvLast;
              for (pvLast++; pvLast < rootMoves.size(); pvLast++)
                  if (rootMoves[pvLast].tbRank != rootMoves[pvFirst].tbRank)
                      break;
          }

          // Reset UCI info selDepth for each depth and each PV line
          selDepth = 0;

          // Reset aspiration window starting size
          Value prev = rootMoves[pvIdx].averageScore;
          delta = Value(10) + int(prev) * prev / 15799;
          alpha = std::max(prev - delta,-VALUE_INFINITE);
          beta  = std::min(prev + delta, VALUE_INFINITE);

          // Adjust optimism based on root move's previousScore
          int opt = 109 * prev / (std::abs(prev) + 141);
          optimism[ us] = Value(opt);
          optimism[~us] = -optimism[us];

          // Start with a small aspiration window and, in the case of a fail
          // high/low, re-search with a bigger window until we don't fail
          // high/low anymore.
          int failedHighCnt = 0;
          while (true)
          {
              // Adjust the effective depth searched, but ensuring at least one effective increment for every
              // four searchAgain steps (see issue #2717).
              Depth adjustedDepth = std::max(1, rootDepth - failedHighCnt - 3 * (searchAgainCounter + 1) / 4);
              bestValue = Stockfish::search<Root>(rootPos, ss, alpha, beta, adjustedDepth, false);

              // Bring the best move to the front. It is critical that sorting
              // is done with a stable algorithm because all the values but the
              // first and eventually the new best one are set to -VALUE_INFINITE
              // and we want to keep the same order for all the moves except the
              // new PV that goes to the front. Note that in case of MultiPV
              // search the already searched PV lines are preserved.
              std::stable_sort(rootMoves.begin() + pvIdx, rootMoves.begin() + pvLast);

              // If search has been stopped, we break immediately. Sorting is
              // safe because RootMoves is still valid, although it refers to
              // the previous iteration.
              if (Threads.stop)
                  break;

              // When failing high/low give some update (without cluttering
              // the UI) before a re-search.
              if (   mainThread
                  && multiPV == 1
                  && (bestValue <= alpha || bestValue >= beta)
                  && Time.elapsed() > 3000)
                  sync_cout << UCI::pv(rootPos, rootDepth) << sync_endl;

              // In case of failing low/high increase aspiration window and
              // re-search, otherwise exit the loop.
              if (bestValue <= alpha)
              {
                  beta = (alpha + beta) / 2;
                  alpha = std::max(bestValue - delta, -VALUE_INFINITE);

                  failedHighCnt = 0;
                  if (mainThread)
                      mainThread->stopOnPonderhit = false;
              }
              else if (bestValue >= beta)
              {
                  beta = std::min(bestValue + delta, VALUE_INFINITE);
                  ++failedHighCnt;
              }
              else
                  break;

              delta += delta / 3;

              assert(alpha >= -VALUE_INFINITE && beta <= VALUE_INFINITE);
          }

          // Sort the PV lines searched so far and update the GUI
          std::stable_sort(rootMoves.begin() + pvFirst, rootMoves.begin() + pvIdx + 1);

          if (    mainThread
              && (Threads.stop || pvIdx + 1 == multiPV || Time.elapsed() > 3000))
              sync_cout << UCI::pv(rootPos, rootDepth) << sync_endl;
      }

      if (!Threads.stop)
          completedDepth = rootDepth;

      if (rootMoves[0].pv[0] != lastBestMove)
      {
          lastBestMove = rootMoves[0].pv[0];
          lastBestMoveDepth = rootDepth;
      }

      // Have we found a "mate in x"?
      if (   Limits.mate
          && bestValue >= VALUE_MATE_IN_MAX_PLY
          && VALUE_MATE - bestValue <= 2 * Limits.mate)
          Threads.stop = true;

      if (!mainThread)
          continue;

      // If skill level is enabled and time is up, pick a sub-optimal best move
      if (skill.enabled() && skill.time_to_pick(rootDepth))
          skill.pick_best(multiPV);

      // Use part of the gained time from a previous stable move for the current move
      for (Thread* th : Threads)
      {
          totBestMoveChanges += th->bestMoveChanges;
          th->bestMoveChanges = 0;
      }

      // Do we have time for the next iteration? Can we stop searching now?
      if (    Limits.use_time_management()
          && !Threads.stop
          && !mainThread->stopOnPonderhit)
      {
          double fallingEval = (69 + 13 * (mainThread->bestPreviousAverageScore - bestValue)
                                    +  6 * (mainThread->iterValue[iterIdx] - bestValue)) / 619.6;
          fallingEval = std::clamp(fallingEval, 0.5, 1.5);

          // If the bestMove is stable over several iterations, reduce time accordingly
          timeReduction = lastBestMoveDepth + 8 < completedDepth ? 1.57 : 0.65;
          double reduction = (1.4 + mainThread->previousTimeReduction) / (2.08 * timeReduction);
          double bestMoveInstability = 1 + 1.8 * totBestMoveChanges / Threads.size();

          // Input features of the neural network (position of pieces)
          size_t k = 0;
          int ft[736];
          for (Square s = SQ_A1; s <= SQ_H8; ++s)
          {
              for (Piece pc = W_PAWN; pc <= B_KING; ++pc)
              {
                  if ((is_non_pawn_rank(s) && type_of(pc) == PAWN) || is_non_piece_type(pc))
                      continue;
                  ft[k] = (us == WHITE) ? rootPos.piece_on(s) == pc
                                        : rootPos.piece_on(rotate_180(s)) == pc;
                  k++;
              }
          }
          // Matrix multiplication
          int neuron[2];
          for (size_t i = 0; i < 2; ++i)
              neuron[i] = std::max(0, std::inner_product(ft, ft+736, nw[i], 0) + nb[i]); // ReLU activation function

          double nn = std::clamp(std::inner_product(neuron, neuron+2, nwo, 0) / 3750.0 + nbo, 0.4, 2.0);

          double totalTime = Time.optimum() * fallingEval * reduction * bestMoveInstability * nn;

          // Cap used time in case of a single legal move for a better viewer experience in tournaments
          // yielding correct scores and sufficiently fast moves.
          if (rootMoves.size() == 1)
              totalTime = std::min(500.0, totalTime);

          // Stop the search if we have exceeded the totalTime
          if (Time.elapsed() > totalTime)
          {
              // If we are allowed to ponder do not stop the search now but
              // keep pondering until the GUI sends "ponderhit" or "stop".
              if (mainThread->ponder)
                  mainThread->stopOnPonderhit = true;
              else
                  Threads.stop = true;
          }
          else if (   !mainThread->ponder
                   && Time.elapsed() > totalTime * 0.50)
              Threads.increaseDepth = false;
          else
              Threads.increaseDepth = true;
      }

      mainThread->iterValue[iterIdx] = bestValue;
      iterIdx = (iterIdx + 1) & 3;
  }

  if (!mainThread)
      return;

  mainThread->previousTimeReduction = timeReduction;

  // If skill level is enabled, swap best PV line with the sub-optimal one
  if (skill.enabled())
      std::swap(rootMoves[0], *std::find(rootMoves.begin(), rootMoves.end(),
                skill.best ? skill.best : skill.pick_best(multiPV)));
}


namespace {

  // search<>() is the main search function for both PV and non-PV nodes

  template <NodeType nodeType>
  Value search(Position& pos, Stack* ss, Value alpha, Value beta, Depth depth, bool cutNode) {

    constexpr bool PvNode = nodeType != NonPV;
    constexpr bool rootNode = nodeType == Root;

    // Check if we have an upcoming move which draws by repetition, or
    // if the opponent had an alternative move earlier to this position.
    if (   !rootNode
        && pos.rule50_count() >= 3
        && alpha < VALUE_DRAW
        && pos.has_game_cycle(ss->ply))
    {
        alpha = value_draw(pos.this_thread());
        if (alpha >= beta)
            return alpha;
    }

    // Dive into quiescence search when the depth reaches zero
    if (depth <= 0)
        return qsearch<PvNode ? PV : NonPV>(pos, ss, alpha, beta);

    assert(-VALUE_INFINITE <= alpha && alpha < beta && beta <= VALUE_INFINITE);
    assert(PvNode || (alpha == beta - 1));
    assert(0 < depth && depth < MAX_PLY);
    assert(!(PvNode && cutNode));

    Move pv[MAX_PLY+1], capturesSearched[32], quietsSearched[64];
    StateInfo st;
    ASSERT_ALIGNED(&st, Eval::NNUE::CacheLineSize);

    TTEntry* tte;
    Key posKey;
    Move ttMove, move, excludedMove, bestMove;
    Depth extension, newDepth;
    Value bestValue, value, ttValue, eval, maxValue, probCutBeta;
    bool givesCheck, improving, priorCapture, singularQuietLMR;
    bool capture, moveCountPruning, ttCapture;
    Piece movedPiece;
    int moveCount, captureCount, quietCount, improvement;

    // Step 1. Initialize node
    Thread* thisThread = pos.this_thread();
    ss->inCheck        = pos.checkers();
    priorCapture       = pos.captured_piece();
    Color us           = pos.side_to_move();
    moveCount          = captureCount = quietCount = ss->moveCount = 0;
    bestValue          = -VALUE_INFINITE;
    maxValue           = VALUE_INFINITE;

    // Check for the available remaining time
    if (thisThread == Threads.main())
        static_cast<MainThread*>(thisThread)->check_time();

    // Used to send selDepth info to GUI (selDepth counts from 1, ply from 0)
    if (PvNode && thisThread->selDepth < ss->ply + 1)
        thisThread->selDepth = ss->ply + 1;

    if (!rootNode)
    {
        // Step 2. Check for aborted search and immediate draw
        if (   Threads.stop.load(std::memory_order_relaxed)
            || pos.is_draw(ss->ply)
            || ss->ply >= MAX_PLY)
            return (ss->ply >= MAX_PLY && !ss->inCheck) ? evaluate(pos)
                                                        : value_draw(pos.this_thread());

        // Step 3. Mate distance pruning. Even if we mate at the next move our score
        // would be at best mate_in(ss->ply+1), but if alpha is already bigger because
        // a shorter mate was found upward in the tree then there is no need to search
        // because we will never beat the current alpha. Same logic but with reversed
        // signs applies also in the opposite condition of being mated instead of giving
        // mate. In this case return a fail-high score.
        alpha = std::max(mated_in(ss->ply), alpha);
        beta = std::min(mate_in(ss->ply+1), beta);
        if (alpha >= beta)
            return alpha;
    }
    else
        thisThread->rootDelta = beta - alpha;

    assert(0 <= ss->ply && ss->ply < MAX_PLY);

    (ss+1)->excludedMove = bestMove = MOVE_NONE;
    (ss+2)->killers[0]   = (ss+2)->killers[1] = MOVE_NONE;
    (ss+2)->cutoffCnt    = 0;
    ss->doubleExtensions = (ss-1)->doubleExtensions;
    Square prevSq        = is_ok((ss-1)->currentMove) ? to_sq((ss-1)->currentMove) : SQ_NONE;
    ss->statScore        = 0;

    // Step 4. Transposition table lookup.
    excludedMove = ss->excludedMove;
    posKey = pos.key();
    tte = TT.probe(posKey, ss->ttHit);
    ttValue = ss->ttHit ? value_from_tt(tte->value(), ss->ply, pos.rule50_count()) : VALUE_NONE;
    ttMove =  rootNode ? thisThread->rootMoves[thisThread->pvIdx].pv[0]
            : ss->ttHit    ? tte->move() : MOVE_NONE;
    ttCapture = ttMove && pos.capture_stage(ttMove);

    // At this point, if excluded, skip straight to step 6, static eval. However,
    // to save indentation, we list the condition in all code between here and there.
    if (!excludedMove)
        ss->ttPv = PvNode || (ss->ttHit && tte->is_pv());

    // At non-PV nodes we check for an early TT cutoff
    if (  !PvNode
        && !excludedMove
        && tte->depth() > depth - (tte->bound() == BOUND_EXACT)
        && ttValue != VALUE_NONE // Possible in case of TT access race or if !ttHit
        && (tte->bound() & (ttValue >= beta ? BOUND_LOWER : BOUND_UPPER)))
    {
        // If ttMove is quiet, update move sorting heuristics on TT hit (~2 Elo)
        if (ttMove)
        {
            if (ttValue >= beta)
            {
                // Bonus for a quiet ttMove that fails high (~2 Elo)
                if (!ttCapture)
                    update_quiet_stats(pos, ss, ttMove, stat_bonus(depth));

                // Extra penalty for early quiet moves of the previous ply (~0 Elo on STC, ~2 Elo on LTC)
                if (prevSq != SQ_NONE && (ss-1)->moveCount <= 2 && !priorCapture)
                    update_continuation_histories(ss-1, pos.piece_on(prevSq), prevSq, -stat_bonus(depth + 1));
            }
            // Penalty for a quiet ttMove that fails low (~1 Elo)
            else if (!ttCapture)
            {
                int penalty = -stat_bonus(depth);
                thisThread->mainHistory[us][from_to(ttMove)] << penalty;
                update_continuation_histories(ss, pos.moved_piece(ttMove), to_sq(ttMove), penalty);
            }
        }

        // Partial workaround for the graph history interaction problem
        // For high rule50 counts don't produce transposition table cutoffs.
        if (pos.rule50_count() < 90)
            return ttValue;
    }

    // Step 5. Tablebases probe
    if (!rootNode && !excludedMove && TB::Cardinality)
    {
        int piecesCount = pos.count<ALL_PIECES>();

        if (    piecesCount <= TB::Cardinality
            && (piecesCount <  TB::Cardinality || depth >= TB::ProbeDepth)
            &&  pos.rule50_count() == 0
            && !pos.can_castle(ANY_CASTLING))
        {
            TB::ProbeState err;
            TB::WDLScore wdl = Tablebases::probe_wdl(pos, &err);

            // Force check of time on the next occasion
            if (thisThread == Threads.main())
                static_cast<MainThread*>(thisThread)->callsCnt = 0;

            if (err != TB::ProbeState::FAIL)
            {
                thisThread->tbHits.fetch_add(1, std::memory_order_relaxed);

                int drawScore = TB::UseRule50 ? 1 : 0;

                // use the range VALUE_MATE_IN_MAX_PLY to VALUE_TB_WIN_IN_MAX_PLY to score
                value =  wdl < -drawScore ? VALUE_MATED_IN_MAX_PLY + ss->ply + 1
                       : wdl >  drawScore ? VALUE_MATE_IN_MAX_PLY - ss->ply - 1
                                          : VALUE_DRAW + 2 * wdl * drawScore;

                Bound b =  wdl < -drawScore ? BOUND_UPPER
                         : wdl >  drawScore ? BOUND_LOWER : BOUND_EXACT;

                if (    b == BOUND_EXACT
                    || (b == BOUND_LOWER ? value >= beta : value <= alpha))
                {
                    tte->save(posKey, value_to_tt(value, ss->ply), ss->ttPv, b,
                              std::min(MAX_PLY - 1, depth + 6),
                              MOVE_NONE, VALUE_NONE);

                    return value;
                }

                if (PvNode)
                {
                    if (b == BOUND_LOWER)
                        bestValue = value, alpha = std::max(alpha, bestValue);
                    else
                        maxValue = value;
                }
            }
        }
    }

    CapturePieceToHistory& captureHistory = thisThread->captureHistory;

    // Step 6. Static evaluation of the position
    if (ss->inCheck)
    {
        // Skip early pruning when in check
        ss->staticEval = eval = VALUE_NONE;
        improving = false;
        improvement = 0;
        goto moves_loop;
    }
    else if (excludedMove)
    {
        // Providing the hint that this node's accumulator will be used often brings significant Elo gain (13 Elo)
        Eval::NNUE::hint_common_parent_position(pos);
        eval = ss->staticEval;
    }
    else if (ss->ttHit)
    {
        // Never assume anything about values stored in TT
        ss->staticEval = eval = tte->eval();
        if (eval == VALUE_NONE)
            ss->staticEval = eval = evaluate(pos);
        else if (PvNode)
            Eval::NNUE::hint_common_parent_position(pos);

        // ttValue can be used as a better position evaluation (~7 Elo)
        if (    ttValue != VALUE_NONE
            && (tte->bound() & (ttValue > eval ? BOUND_LOWER : BOUND_UPPER)))
            eval = ttValue;
    }
    else
    {
        ss->staticEval = eval = evaluate(pos);
        // Save static evaluation into transposition table
        tte->save(posKey, VALUE_NONE, ss->ttPv, BOUND_NONE, DEPTH_NONE, MOVE_NONE, eval);
    }

    // Use static evaluation difference to improve quiet move ordering (~4 Elo)
    if (is_ok((ss-1)->currentMove) && !(ss-1)->inCheck && !priorCapture)
    {
        int bonus = std::clamp(-18 * int((ss-1)->staticEval + ss->staticEval), -1817, 1817);
        thisThread->mainHistory[~us][from_to((ss-1)->currentMove)] << bonus;
    }

    // Set up the improvement variable, which is the difference between the current
    // static evaluation and the previous static evaluation at our turn (if we were
    // in check at our previous move we look at the move prior to it). The improvement
    // margin and the improving flag are used in various pruning heuristics.
    improvement =   (ss-2)->staticEval != VALUE_NONE ? ss->staticEval - (ss-2)->staticEval
                  : (ss-4)->staticEval != VALUE_NONE ? ss->staticEval - (ss-4)->staticEval
                  :                                    173;
    improving = improvement > 0;

    // Step 7. Razoring (~1 Elo).
    // If eval is really low check with qsearch if it can exceed alpha, if it can't,
    // return a fail low.
    if (eval < alpha - 456 - 252 * depth * depth)
    {
        value = qsearch<NonPV>(pos, ss, alpha - 1, alpha);
        if (value < alpha)
            return value;
    }

    // Step 8. Futility pruning: child node (~40 Elo).
    // The depth condition is important for mate finding.
    if (   !ss->ttPv
        &&  depth < 9
        &&  eval - futility_margin(depth, improving) - (ss-1)->statScore / 306 >= beta
        &&  eval >= beta
        &&  eval < 24923) // larger than VALUE_KNOWN_WIN, but smaller than TB wins
        return eval;

    // Step 9. Null move search with verification search (~35 Elo)
    if (   !PvNode
        && (ss-1)->currentMove != MOVE_NULL
        && (ss-1)->statScore < 17329
        &&  eval >= beta
        &&  eval >= ss->staticEval
        &&  ss->staticEval >= beta - 21 * depth - improvement * 99 / 1300 + 258
        && !excludedMove
        &&  pos.non_pawn_material(us)
        && (ss->ply >= thisThread->nmpMinPly))
    {
        assert(eval - beta >= 0);

        // Null move dynamic reduction based on depth and eval
        Depth R = std::min(int(eval - beta) / 173, 6) + depth / 3 + 4;

        ss->currentMove = MOVE_NULL;
        ss->continuationHistory = &thisThread->continuationHistory[0][0][NO_PIECE][0];

        pos.do_null_move(st);

        Value nullValue = -search<NonPV>(pos, ss+1, -beta, -beta+1, depth-R, !cutNode);

        pos.undo_null_move();

        if (nullValue >= beta)
        {
            // Do not return unproven mate or TB scores
            if (nullValue >= VALUE_TB_WIN_IN_MAX_PLY)
                nullValue = beta;

            if (thisThread->nmpMinPly || (abs(beta) < VALUE_KNOWN_WIN && depth < 14))
                return nullValue;

            assert(!thisThread->nmpMinPly); // Recursive verification is not allowed

            // Do verification search at high depths, with null move pruning disabled
            // until ply exceeds nmpMinPly.
            thisThread->nmpMinPly = ss->ply + 3 * (depth-R) / 4;

            Value v = search<NonPV>(pos, ss, beta-1, beta, depth-R, false);

            thisThread->nmpMinPly = 0;

            if (v >= beta)
                return nullValue;
        }
    }

    // Step 10. If the position doesn't a have ttMove, decrease depth by 2
    // (or by 4 if the TT entry for the current position was hit and the stored depth is greater than or equal to the current depth).
    // Use qsearch if depth is equal or below zero (~9 Elo)
    if (    PvNode
        && !ttMove)
        depth -= 2 + 2 * (ss->ttHit && tte->depth() >= depth);

    if (depth <= 0)
        return qsearch<PV>(pos, ss, alpha, beta);

    if (    cutNode
        &&  depth >= 8
        && !ttMove)
        depth -= 2;

    probCutBeta = beta + 168 - 61 * improving;

    // Step 11. ProbCut (~10 Elo)
    // If we have a good enough capture (or queen promotion) and a reduced search returns a value
    // much above beta, we can (almost) safely prune the previous move.
    if (   !PvNode
        &&  depth > 3
        &&  abs(beta) < VALUE_TB_WIN_IN_MAX_PLY
        // if value from transposition table is lower than probCutBeta, don't attempt probCut
        // there and in further interactions with transposition table cutoff depth is set to depth - 3
        // because probCut search has depth set to depth - 4 but we also do a move before it
        // so effective depth is equal to depth - 3
        && !(   tte->depth() >= depth - 3
             && ttValue != VALUE_NONE
             && ttValue < probCutBeta))
    {
        assert(probCutBeta < VALUE_INFINITE);

        MovePicker mp(pos, ttMove, probCutBeta - ss->staticEval, &captureHistory);

        while ((move = mp.next_move()) != MOVE_NONE)
            if (move != excludedMove && pos.legal(move))
            {
                assert(pos.capture_stage(move));

                ss->currentMove = move;
                ss->continuationHistory = &thisThread->continuationHistory[ss->inCheck]
                                                                          [true]
                                                                          [pos.moved_piece(move)]
                                                                          [to_sq(move)];

                pos.do_move(move, st);

                // Perform a preliminary qsearch to verify that the move holds
                value = -qsearch<NonPV>(pos, ss+1, -probCutBeta, -probCutBeta+1);

                // If the qsearch held, perform the regular search
                if (value >= probCutBeta)
                    value = -search<NonPV>(pos, ss+1, -probCutBeta, -probCutBeta+1, depth - 4, !cutNode);

                pos.undo_move(move);

                if (value >= probCutBeta)
                {
                    // Save ProbCut data into transposition table
                    tte->save(posKey, value_to_tt(value, ss->ply), ss->ttPv, BOUND_LOWER, depth - 3, move, ss->staticEval);
                    return value;
                }
            }

        Eval::NNUE::hint_common_parent_position(pos);
    }

moves_loop: // When in check, search starts here

    // Step 12. A small Probcut idea, when we are in check (~4 Elo)
    probCutBeta = beta + 413;
    if (   ss->inCheck
        && !PvNode
        && ttCapture
        && (tte->bound() & BOUND_LOWER)
        && tte->depth() >= depth - 4
        && ttValue >= probCutBeta
        && abs(ttValue) <= VALUE_KNOWN_WIN
        && abs(beta) <= VALUE_KNOWN_WIN)
        return probCutBeta;

    const PieceToHistory* contHist[] = { (ss-1)->continuationHistory, (ss-2)->continuationHistory,
                                          nullptr                   , (ss-4)->continuationHistory,
                                          nullptr                   , (ss-6)->continuationHistory };

    Move countermove = prevSq != SQ_NONE ? thisThread->counterMoves[pos.piece_on(prevSq)][prevSq] : MOVE_NONE;

    MovePicker mp(pos, ttMove, depth, &thisThread->mainHistory,
                                      &captureHistory,
                                      contHist,
                                      countermove,
                                      ss->killers);

    value = bestValue;
    moveCountPruning = singularQuietLMR = false;

    // Indicate PvNodes that will probably fail low if the node was searched
    // at a depth equal or greater than the current depth, and the result of this search was a fail low.
    bool likelyFailLow =    PvNode
                         && ttMove
                         && (tte->bound() & BOUND_UPPER)
                         && tte->depth() >= depth;

    // Step 13. Loop through all pseudo-legal moves until no moves remain
    // or a beta cutoff occurs.
    while ((move = mp.next_move(moveCountPruning)) != MOVE_NONE)
    {
      assert(is_ok(move));

      if (move == excludedMove)
          continue;

      // At root obey the "searchmoves" option and skip moves not listed in Root
      // Move List. As a consequence any illegal move is also skipped. In MultiPV
      // mode we also skip PV moves which have been already searched and those
      // of lower "TB rank" if we are in a TB root position.
      if (rootNode && !std::count(thisThread->rootMoves.begin() + thisThread->pvIdx,
                                  thisThread->rootMoves.begin() + thisThread->pvLast, move))
          continue;

      // Check for legality
      if (!rootNode && !pos.legal(move))
          continue;

      ss->moveCount = ++moveCount;

      if (rootNode && thisThread == Threads.main() && Time.elapsed() > 3000)
          sync_cout << "info depth " << depth
                    << " currmove " << UCI::move(move, pos.is_chess960())
                    << " currmovenumber " << moveCount + thisThread->pvIdx << sync_endl;
      if (PvNode)
          (ss+1)->pv = nullptr;

      extension = 0;
      capture = pos.capture_stage(move);
      movedPiece = pos.moved_piece(move);
      givesCheck = pos.gives_check(move);

      // Calculate new depth for this move
      newDepth = depth - 1;

      Value delta = beta - alpha;

      Depth r = reduction(improving, depth, moveCount, delta, thisThread->rootDelta);

      // Step 14. Pruning at shallow depth (~120 Elo). Depth conditions are important for mate finding.
      if (  !rootNode
          && pos.non_pawn_material(us)
          && bestValue > VALUE_TB_LOSS_IN_MAX_PLY)
      {
          // Skip quiet moves if movecount exceeds our FutilityMoveCount threshold (~8 Elo)
          moveCountPruning = moveCount >= futility_move_count(improving, depth);

          // Reduced depth of the next LMR search
          int lmrDepth = newDepth - r;

          if (   capture
              || givesCheck)
          {
              // Futility pruning for captures (~2 Elo)
              if (   !givesCheck
                  && lmrDepth < 7
                  && !ss->inCheck
                  && ss->staticEval + 197 + 248 * lmrDepth + PieceValue[EG][pos.piece_on(to_sq(move))]
                   + captureHistory[movedPiece][to_sq(move)][type_of(pos.piece_on(to_sq(move)))] / 7 < alpha)
                  continue;

              // SEE based pruning (~11 Elo)
              if (!pos.see_ge(move, Value(-205) * depth))
                      continue;
          }
          else
          {
              int history =   (*contHist[0])[movedPiece][to_sq(move)]
                            + (*contHist[1])[movedPiece][to_sq(move)]
                            + (*contHist[3])[movedPiece][to_sq(move)];

              // Continuation history based pruning (~2 Elo)
              if (   lmrDepth < 6
                  && history < -3832 * depth)
                  continue;

              history += 2 * thisThread->mainHistory[us][from_to(move)];

              lmrDepth += history / 7011;
              lmrDepth = std::max(lmrDepth, -2);

              // Futility pruning: parent node (~13 Elo)
              if (   !ss->inCheck
                  && lmrDepth < 12
                  && ss->staticEval + 112 + 138 * lmrDepth <= alpha)
                  continue;

              lmrDepth = std::max(lmrDepth, 0);

              // Prune moves with negative SEE (~4 Elo)
              if (!pos.see_ge(move, Value(-27 * lmrDepth * lmrDepth - 16 * lmrDepth)))
                  continue;
          }
      }

      // Step 15. Extensions (~100 Elo)
      // We take care to not overdo to avoid search getting stuck.
      if (ss->ply < thisThread->rootDepth * 2)
      {
          // Singular extension search (~94 Elo). If all moves but one fail low on a
          // search of (alpha-s, beta-s), and just one fails high on (alpha, beta),
          // then that move is singular and should be extended. To verify this we do
          // a reduced search on all the other moves but the ttMove and if the
          // result is lower than ttValue minus a margin, then we will extend the ttMove.
          if (   !rootNode
              &&  depth >= 4 - (thisThread->completedDepth > 22) + 2 * (PvNode && tte->is_pv())
              &&  move == ttMove
              && !excludedMove // Avoid recursive singular search
           /* &&  ttValue != VALUE_NONE Already implicit in the next condition */
              &&  abs(ttValue) < VALUE_KNOWN_WIN
              && (tte->bound() & BOUND_LOWER)
              &&  tte->depth() >= depth - 3)
          {
              Value singularBeta = ttValue - (82 + 65 * (ss->ttPv && !PvNode)) * depth / 64;
              Depth singularDepth = (depth - 1) / 2;

              ss->excludedMove = move;
              value = search<NonPV>(pos, ss, singularBeta - 1, singularBeta, singularDepth, cutNode);
              ss->excludedMove = MOVE_NONE;

              if (value < singularBeta)
              {
                  extension = 1;
                  singularQuietLMR = !ttCapture;

                  // Avoid search explosion by limiting the number of double extensions
                  if (  !PvNode
                      && value < singularBeta - 21
                      && ss->doubleExtensions <= 11)
                  {
                      extension = 2;
                      depth += depth < 13;
                  }
              }

              // Multi-cut pruning
              // Our ttMove is assumed to fail high, and now we failed high also on a reduced
              // search without the ttMove. So we assume this expected Cut-node is not singular,
              // that multiple moves fail high, and we can prune the whole subtree by returning
              // a soft bound.
              else if (singularBeta >= beta)
                  return singularBeta;

              // If the eval of ttMove is greater than beta, we reduce it (negative extension) (~7 Elo)
              else if (ttValue >= beta)
                  extension = -2 - !PvNode;

              // If the eval of ttMove is less than value, we reduce it (negative extension) (~1 Elo)
              else if (ttValue <= value)
                  extension = -1;

              // If the eval of ttMove is less than alpha, we reduce it (negative extension) (~1 Elo)
              else if (ttValue <= alpha)
                  extension = -1;
          }

          // Check extensions (~1 Elo)
          else if (   givesCheck
                   && depth > 9)
              extension = 1;

          // Quiet ttMove extensions (~1 Elo)
          else if (   PvNode
                   && move == ttMove
                   && move == ss->killers[0]
                   && (*contHist[0])[movedPiece][to_sq(move)] >= 5168)
              extension = 1;
      }

      // Add extension to new depth
      newDepth += extension;
      ss->doubleExtensions = (ss-1)->doubleExtensions + (extension == 2);

      // Speculative prefetch as early as possible
      prefetch(TT.first_entry(pos.key_after(move)));

      // Update the current move (this must be done after singular extension search)
      ss->currentMove = move;
      ss->continuationHistory = &thisThread->continuationHistory[ss->inCheck]
                                                                [capture]
                                                                [movedPiece]
                                                                [to_sq(move)];

      // Step 16. Make the move
      pos.do_move(move, st, givesCheck);

      // Decrease reduction if position is or has been on the PV
      // and node is not likely to fail low. (~3 Elo)
      // Decrease further on cutNodes. (~1 Elo)
      if (   ss->ttPv
          && !likelyFailLow)
          r -= cutNode && tte->depth() >= depth + 3 ? 3 : 2;

      // Decrease reduction if opponent's move count is high (~1 Elo)
      if ((ss-1)->moveCount > 8)
          r--;

      // Increase reduction for cut nodes (~3 Elo)
      if (cutNode)
          r += 2;

      // Increase reduction if ttMove is a capture (~3 Elo)
      if (ttCapture)
          r++;

      // Decrease reduction for PvNodes based on depth (~2 Elo)
      if (PvNode)
          r -= 1 + 12 / (3 + depth);

      // Decrease reduction if ttMove has been singularly extended (~1 Elo)
      if (singularQuietLMR)
          r--;

      // Increase reduction if next ply has a lot of fail high (~5 Elo)
      if ((ss+1)->cutoffCnt > 3)
          r++;

      else if (move == ttMove)
          r--;

      ss->statScore =  2 * thisThread->mainHistory[us][from_to(move)]
                     + (*contHist[0])[movedPiece][to_sq(move)]
                     + (*contHist[1])[movedPiece][to_sq(move)]
                     + (*contHist[3])[movedPiece][to_sq(move)]
                     - 4006;

      // Decrease/increase reduction for moves with a good/bad history (~25 Elo)
      r -= ss->statScore / (11124 + 4740 * (depth > 5 && depth < 22));

      // Step 17. Late moves reduction / extension (LMR, ~117 Elo)
      // We use various heuristics for the sons of a node after the first son has
      // been searched. In general we would like to reduce them, but there are many
      // cases where we extend a son if it has good chances to be "interesting".
      if (    depth >= 2
          &&  moveCount > 1 + (PvNode && ss->ply <= 1)
          && (   !ss->ttPv
              || !capture
              || (cutNode && (ss-1)->moveCount > 1)))
      {
          // In general we want to cap the LMR depth search at newDepth, but when
          // reduction is negative, we allow this move a limited search extension
          // beyond the first move depth. This may lead to hidden double extensions.
          Depth d = std::clamp(newDepth - r, 1, newDepth + 1);

          value = -search<NonPV>(pos, ss+1, -(alpha+1), -alpha, d, true);

          // Do full depth search when reduced LMR search fails high
          if (value > alpha && d < newDepth)
          {
              // Adjust full depth search based on LMR results - if result
              // was good enough search deeper, if it was bad enough search shallower
              const bool doDeeperSearch = value > (bestValue + 64 + 11 * (newDepth - d));
              const bool doEvenDeeperSearch = value > alpha + 711 && ss->doubleExtensions <= 6;
              const bool doShallowerSearch = value < bestValue + newDepth;

              ss->doubleExtensions = ss->doubleExtensions + doEvenDeeperSearch;

              newDepth += doDeeperSearch - doShallowerSearch + doEvenDeeperSearch;

              if (newDepth > d)
                  value = -search<NonPV>(pos, ss+1, -(alpha+1), -alpha, newDepth, !cutNode);

              int bonus = value <= alpha ? -stat_bonus(newDepth)
                        : value >= beta  ?  stat_bonus(newDepth)
                                         :  0;

              update_continuation_histories(ss, movedPiece, to_sq(move), bonus);
          }
      }

      // Step 18. Full depth search when LMR is skipped. If expected reduction is high, reduce its depth by 1.
      else if (!PvNode || moveCount > 1)
      {
          // Increase reduction for cut nodes and not ttMove (~1 Elo)
          if (!ttMove && cutNode)
              r += 2;

          value = -search<NonPV>(pos, ss+1, -(alpha+1), -alpha, newDepth - (r > 3), !cutNode);
      }

      // For PV nodes only, do a full PV search on the first move or after a fail
      // high (in the latter case search only if value < beta), otherwise let the
      // parent node fail low with value <= alpha and try another move.
      if (PvNode && (moveCount == 1 || (value > alpha && (rootNode || value < beta))))
      {
          (ss+1)->pv = pv;
          (ss+1)->pv[0] = MOVE_NONE;

          value = -search<PV>(pos, ss+1, -beta, -alpha, newDepth, false);
      }

      // Step 19. Undo move
      pos.undo_move(move);

      assert(value > -VALUE_INFINITE && value < VALUE_INFINITE);

      // Step 20. Check for a new best move
      // Finished searching the move. If a stop occurred, the return value of
      // the search cannot be trusted, and we return immediately without
      // updating best move, PV and TT.
      if (Threads.stop.load(std::memory_order_relaxed))
          return VALUE_ZERO;

      if (rootNode)
      {
          RootMove& rm = *std::find(thisThread->rootMoves.begin(),
                                    thisThread->rootMoves.end(), move);

          rm.averageScore = rm.averageScore != -VALUE_INFINITE ? (2 * value + rm.averageScore) / 3 : value;

          // PV move or new best move?
          if (moveCount == 1 || value > alpha)
          {
              rm.score =  rm.uciScore = value;
              rm.selDepth = thisThread->selDepth;
              rm.scoreLowerbound = rm.scoreUpperbound = false;

              if (value >= beta)
              {
                  rm.scoreLowerbound = true;
                  rm.uciScore = beta;
              }
              else if (value <= alpha)
              {
                  rm.scoreUpperbound = true;
                  rm.uciScore = alpha;
              }

              rm.pv.resize(1);

              assert((ss+1)->pv);

              for (Move* m = (ss+1)->pv; *m != MOVE_NONE; ++m)
                  rm.pv.push_back(*m);

              // We record how often the best move has been changed in each iteration.
              // This information is used for time management. In MultiPV mode,
              // we must take care to only do this for the first PV line.
              if (   moveCount > 1
                  && !thisThread->pvIdx)
                  ++thisThread->bestMoveChanges;
          }
          else
              // All other moves but the PV are set to the lowest value: this
              // is not a problem when sorting because the sort is stable and the
              // move position in the list is preserved - just the PV is pushed up.
              rm.score = -VALUE_INFINITE;
      }

      if (value > bestValue)
      {
          bestValue = value;

          if (value > alpha)
          {
              bestMove = move;

              if (PvNode && !rootNode) // Update pv even in fail-high case
                  update_pv(ss->pv, move, (ss+1)->pv);

              if (value >= beta)
              {
                  ss->cutoffCnt += 1 + !ttMove;
                  assert(value >= beta); // Fail high
                  break;
              }
              else
              {
                  // Reduce other moves if we have found at least one score improvement (~1 Elo)
                  // Reduce more for depth > 3 and depth < 12 (~1 Elo)
                  if (   depth > 1
                      && beta  <  14362
                      && value > -12393)
                      depth -= depth > 3 && depth < 12 ? 2 : 1;

                  assert(depth > 0);
                  alpha = value; // Update alpha! Always alpha < beta
              }
          }
      }


      // If the move is worse than some previously searched move, remember it to update its stats later
      if (move != bestMove)
      {
          if (capture && captureCount < 32)
              capturesSearched[captureCount++] = move;

          else if (!capture && quietCount < 64)
              quietsSearched[quietCount++] = move;
      }
    }

    // The following condition would detect a stop only after move loop has been
    // completed. But in this case bestValue is valid because we have fully
    // searched our subtree, and we can anyhow save the result in TT.
    /*
       if (Threads.stop)
        return VALUE_DRAW;
    */

    // Step 21. Check for mate and stalemate
    // All legal moves have been searched and if there are no legal moves, it
    // must be a mate or a stalemate. If we are in a singular extension search then
    // return a fail low score.

    assert(moveCount || !ss->inCheck || excludedMove || !MoveList<LEGAL>(pos).size());

    if (!moveCount)
        bestValue = excludedMove ? alpha :
                    ss->inCheck  ? mated_in(ss->ply)
                                 : VALUE_DRAW;

    // If there is a move which produces search value greater than alpha we update stats of searched moves
    else if (bestMove)
        update_all_stats(pos, ss, bestMove, bestValue, beta, prevSq,
                         quietsSearched, quietCount, capturesSearched, captureCount, depth);

    // Bonus for prior countermove that caused the fail low
    else if (!priorCapture && prevSq != SQ_NONE)
    {
        int bonus = (depth > 5) + (PvNode || cutNode) + (bestValue < alpha - 113 * depth) + ((ss-1)->moveCount > 12);
        update_continuation_histories(ss-1, pos.piece_on(prevSq), prevSq, stat_bonus(depth) * bonus);
    }

    if (PvNode)
        bestValue = std::min(bestValue, maxValue);

    // If no good move is found and the previous position was ttPv, then the previous
    // opponent move is probably good and the new position is added to the search tree. (~7 Elo)
    if (bestValue <= alpha)
        ss->ttPv = ss->ttPv || ((ss-1)->ttPv && depth > 3);

    // Write gathered information in transposition table
    if (!excludedMove && !(rootNode && thisThread->pvIdx))
        tte->save(posKey, value_to_tt(bestValue, ss->ply), ss->ttPv,
                  bestValue >= beta ? BOUND_LOWER :
                  PvNode && bestMove ? BOUND_EXACT : BOUND_UPPER,
                  depth, bestMove, ss->staticEval);

    assert(bestValue > -VALUE_INFINITE && bestValue < VALUE_INFINITE);

    return bestValue;
  }


  // qsearch() is the quiescence search function, which is called by the main search
  // function with zero depth, or recursively with further decreasing depth per call.
  // (~155 Elo)
  template <NodeType nodeType>
  Value qsearch(Position& pos, Stack* ss, Value alpha, Value beta, Depth depth) {

    static_assert(nodeType != Root);
    constexpr bool PvNode = nodeType == PV;

    assert(alpha >= -VALUE_INFINITE && alpha < beta && beta <= VALUE_INFINITE);
    assert(PvNode || (alpha == beta - 1));
    assert(depth <= 0);

    Move pv[MAX_PLY+1];
    StateInfo st;
    ASSERT_ALIGNED(&st, Eval::NNUE::CacheLineSize);

    TTEntry* tte;
    Key posKey;
    Move ttMove, move, bestMove;
    Depth ttDepth;
    Value bestValue, value, ttValue, futilityValue, futilityBase;
    bool pvHit, givesCheck, capture;
    int moveCount;

    // Step 1. Initialize node
    if (PvNode)
    {
        (ss+1)->pv = pv;
        ss->pv[0] = MOVE_NONE;
    }

    Thread* thisThread = pos.this_thread();
    bestMove = MOVE_NONE;
    ss->inCheck = pos.checkers();
    moveCount = 0;

    // Step 2. Check for an immediate draw or maximum ply reached
    if (   pos.is_draw(ss->ply)
        || ss->ply >= MAX_PLY)
        return (ss->ply >= MAX_PLY && !ss->inCheck) ? evaluate(pos) : VALUE_DRAW;

    assert(0 <= ss->ply && ss->ply < MAX_PLY);

    // Decide whether or not to include checks: this fixes also the type of
    // TT entry depth that we are going to use. Note that in qsearch we use
    // only two types of depth in TT: DEPTH_QS_CHECKS or DEPTH_QS_NO_CHECKS.
    ttDepth = ss->inCheck || depth >= DEPTH_QS_CHECKS ? DEPTH_QS_CHECKS
                                                      : DEPTH_QS_NO_CHECKS;

    // Step 3. Transposition table lookup
    posKey = pos.key();
    tte = TT.probe(posKey, ss->ttHit);
    ttValue = ss->ttHit ? value_from_tt(tte->value(), ss->ply, pos.rule50_count()) : VALUE_NONE;
    ttMove = ss->ttHit ? tte->move() : MOVE_NONE;
    pvHit = ss->ttHit && tte->is_pv();

    // At non-PV nodes we check for an early TT cutoff
    if (  !PvNode
        && tte->depth() >= ttDepth
        && ttValue != VALUE_NONE // Only in case of TT access race or if !ttHit
        && (tte->bound() & (ttValue >= beta ? BOUND_LOWER : BOUND_UPPER)))
        return ttValue;

    // Step 4. Static evaluation of the position
    if (ss->inCheck)
    {
        ss->staticEval = VALUE_NONE;
        bestValue = futilityBase = -VALUE_INFINITE;
    }
    else
    {
        if (ss->ttHit)
        {
            // Never assume anything about values stored in TT
            if ((ss->staticEval = bestValue = tte->eval()) == VALUE_NONE)
                ss->staticEval = bestValue = evaluate(pos);

            // ttValue can be used as a better position evaluation (~13 Elo)
            if (    ttValue != VALUE_NONE
                && (tte->bound() & (ttValue > bestValue ? BOUND_LOWER : BOUND_UPPER)))
                bestValue = ttValue;
        }
        else
            // In case of null move search use previous static eval with a different sign
            ss->staticEval = bestValue =
            (ss-1)->currentMove != MOVE_NULL ? evaluate(pos)
                                             : -(ss-1)->staticEval;

        // Stand pat. Return immediately if static value is at least beta
        if (bestValue >= beta)
        {
            // Save gathered info in transposition table
            if (!ss->ttHit)
                tte->save(posKey, value_to_tt(bestValue, ss->ply), false, BOUND_LOWER,
                          DEPTH_NONE, MOVE_NONE, ss->staticEval);

            return bestValue;
        }

        if (PvNode && bestValue > alpha)
            alpha = bestValue;

        futilityBase = bestValue + 200;
    }

    const PieceToHistory* contHist[] = { (ss-1)->continuationHistory, (ss-2)->continuationHistory,
                                          nullptr                   , (ss-4)->continuationHistory,
                                          nullptr                   , (ss-6)->continuationHistory };

    // Initialize a MovePicker object for the current position, and prepare
    // to search the moves. Because the depth is <= 0 here, only captures,
    // queen promotions, and other checks (only if depth >= DEPTH_QS_CHECKS)
    // will be generated.
    Square prevSq = (ss-1)->currentMove != MOVE_NULL ? to_sq((ss-1)->currentMove) : SQ_NONE;
    MovePicker mp(pos, ttMove, depth, &thisThread->mainHistory,
                                      &thisThread->captureHistory,
                                      contHist,
                                      prevSq);

    int quietCheckEvasions = 0;

    // Step 5. Loop through all pseudo-legal moves until no moves remain
    // or a beta cutoff occurs.
    while ((move = mp.next_move()) != MOVE_NONE)
    {
      assert(is_ok(move));

      // Check for legality
      if (!pos.legal(move))
          continue;

      givesCheck = pos.gives_check(move);
      capture = pos.capture_stage(move);

      moveCount++;

    // Step 6. Pruning.
    if (bestValue > VALUE_TB_LOSS_IN_MAX_PLY)
    {
      // Futility pruning and moveCount pruning (~10 Elo)
      if (   !givesCheck
          &&  to_sq(move) != prevSq
          &&  futilityBase > -VALUE_KNOWN_WIN
          &&  type_of(move) != PROMOTION)
      {
          if (moveCount > 2)
              continue;

          futilityValue = futilityBase + PieceValue[EG][pos.piece_on(to_sq(move))];

          if (futilityValue <= alpha)
          {
              bestValue = std::max(bestValue, futilityValue);
              continue;
          }

          if (futilityBase <= alpha && !pos.see_ge(move, VALUE_ZERO + 1))
          {
              bestValue = std::max(bestValue, futilityBase);
              continue;
          }
      }

      // We prune after 2nd quiet check evasion where being 'in check' is implicitly checked through the counter
      // and being a 'quiet' apart from being a tt move is assumed after an increment because captures are pushed ahead.
      if (quietCheckEvasions > 1)
          break;

      // Continuation history based pruning (~3 Elo)
      if (   !capture
          && (*contHist[0])[pos.moved_piece(move)][to_sq(move)] < 0
          && (*contHist[1])[pos.moved_piece(move)][to_sq(move)] < 0)
          continue;

      // Do not search moves with bad enough SEE values (~5 Elo)
      if (!pos.see_ge(move, Value(-95)))
          continue;
    }

      // Speculative prefetch as early as possible
      prefetch(TT.first_entry(pos.key_after(move)));

      // Update the current move
      ss->currentMove = move;
      ss->continuationHistory = &thisThread->continuationHistory[ss->inCheck]
                                                                [capture]
                                                                [pos.moved_piece(move)]
                                                                [to_sq(move)];

      quietCheckEvasions += !capture && ss->inCheck;

      // Step 7. Make and search the move
      pos.do_move(move, st, givesCheck);
      value = -qsearch<nodeType>(pos, ss+1, -beta, -alpha, depth - 1);
      pos.undo_move(move);

      assert(value > -VALUE_INFINITE && value < VALUE_INFINITE);

      // Step 8. Check for a new best move
      if (value > bestValue)
      {
          bestValue = value;

          if (value > alpha)
          {
              bestMove = move;

              if (PvNode) // Update pv even in fail-high case
                  update_pv(ss->pv, move, (ss+1)->pv);

              if (PvNode && value < beta) // Update alpha here!
                  alpha = value;
              else
                  break; // Fail high
          }
       }
    }

    // Step 9. Check for mate
    // All legal moves have been searched. A special case: if we're in check
    // and no legal moves were found, it is checkmate.
    if (ss->inCheck && bestValue == -VALUE_INFINITE)
    {
        assert(!MoveList<LEGAL>(pos).size());

        return mated_in(ss->ply); // Plies to mate from the root
    }

    // Save gathered info in transposition table
    tte->save(posKey, value_to_tt(bestValue, ss->ply), pvHit,
              bestValue >= beta ? BOUND_LOWER : BOUND_UPPER,
              ttDepth, bestMove, ss->staticEval);

    assert(bestValue > -VALUE_INFINITE && bestValue < VALUE_INFINITE);

    return bestValue;
  }


  // value_to_tt() adjusts a mate or TB score from "plies to mate from the root" to
  // "plies to mate from the current position". Standard scores are unchanged.
  // The function is called before storing a value in the transposition table.

  Value value_to_tt(Value v, int ply) {

    assert(v != VALUE_NONE);

    return  v >= VALUE_TB_WIN_IN_MAX_PLY  ? v + ply
          : v <= VALUE_TB_LOSS_IN_MAX_PLY ? v - ply : v;
  }


  // value_from_tt() is the inverse of value_to_tt(): it adjusts a mate or TB score
  // from the transposition table (which refers to the plies to mate/be mated from
  // current position) to "plies to mate/be mated (TB win/loss) from the root". However,
  // for mate scores, to avoid potentially false mate scores related to the 50 moves rule
  // and the graph history interaction, we return an optimal TB score instead.

  Value value_from_tt(Value v, int ply, int r50c) {

    if (v == VALUE_NONE)
        return VALUE_NONE;

    if (v >= VALUE_TB_WIN_IN_MAX_PLY)  // TB win or better
    {
        if (v >= VALUE_MATE_IN_MAX_PLY && VALUE_MATE - v > 99 - r50c)
            return VALUE_MATE_IN_MAX_PLY - 1; // do not return a potentially false mate score

        return v - ply;
    }

    if (v <= VALUE_TB_LOSS_IN_MAX_PLY) // TB loss or worse
    {
        if (v <= VALUE_MATED_IN_MAX_PLY && VALUE_MATE + v > 99 - r50c)
            return VALUE_MATED_IN_MAX_PLY + 1; // do not return a potentially false mate score

        return v + ply;
    }

    return v;
  }


  // update_pv() adds current move and appends child pv[]

  void update_pv(Move* pv, Move move, const Move* childPv) {

    for (*pv++ = move; childPv && *childPv != MOVE_NONE; )
        *pv++ = *childPv++;
    *pv = MOVE_NONE;
  }


  // update_all_stats() updates stats at the end of search() when a bestMove is found

  void update_all_stats(const Position& pos, Stack* ss, Move bestMove, Value bestValue, Value beta, Square prevSq,
                        Move* quietsSearched, int quietCount, Move* capturesSearched, int captureCount, Depth depth) {

    Color us = pos.side_to_move();
    Thread* thisThread = pos.this_thread();
    CapturePieceToHistory& captureHistory = thisThread->captureHistory;
    Piece moved_piece = pos.moved_piece(bestMove);
    PieceType captured;

    int bonus1 = stat_bonus(depth + 1);

    if (!pos.capture_stage(bestMove))
    {
        int bonus2 = bestValue > beta + 145 ? bonus1               // larger bonus
                                            : stat_bonus(depth);   // smaller bonus

        // Increase stats for the best move in case it was a quiet move
        update_quiet_stats(pos, ss, bestMove, bonus2);

        // Decrease stats for all non-best quiet moves
        for (int i = 0; i < quietCount; ++i)
        {
            thisThread->mainHistory[us][from_to(quietsSearched[i])] << -bonus2;
            update_continuation_histories(ss, pos.moved_piece(quietsSearched[i]), to_sq(quietsSearched[i]), -bonus2);
        }
    }
    else
    {
        // Increase stats for the best move in case it was a capture move
        captured = type_of(pos.piece_on(to_sq(bestMove)));
        captureHistory[moved_piece][to_sq(bestMove)][captured] << bonus1;
    }

    // Extra penalty for a quiet early move that was not a TT move or
    // main killer move in previous ply when it gets refuted.
    if (   prevSq != SQ_NONE
        && ((ss-1)->moveCount == 1 + (ss-1)->ttHit || ((ss-1)->currentMove == (ss-1)->killers[0]))
        && !pos.captured_piece())
            update_continuation_histories(ss-1, pos.piece_on(prevSq), prevSq, -bonus1);

    // Decrease stats for all non-best capture moves
    for (int i = 0; i < captureCount; ++i)
    {
        moved_piece = pos.moved_piece(capturesSearched[i]);
        captured = type_of(pos.piece_on(to_sq(capturesSearched[i])));
        captureHistory[moved_piece][to_sq(capturesSearched[i])][captured] << -bonus1;
    }
  }


  // update_continuation_histories() updates histories of the move pairs formed
  // by moves at ply -1, -2, -4, and -6 with current move.

  void update_continuation_histories(Stack* ss, Piece pc, Square to, int bonus) {

    for (int i : {1, 2, 4, 6})
    {
        // Only update first 2 continuation histories if we are in check
        if (ss->inCheck && i > 2)
            break;
        if (is_ok((ss-i)->currentMove))
            (*(ss-i)->continuationHistory)[pc][to] << bonus;
    }
  }


  // update_quiet_stats() updates move sorting heuristics

  void update_quiet_stats(const Position& pos, Stack* ss, Move move, int bonus) {

    // Update killers
    if (ss->killers[0] != move)
    {
        ss->killers[1] = ss->killers[0];
        ss->killers[0] = move;
    }

    Color us = pos.side_to_move();
    Thread* thisThread = pos.this_thread();
    thisThread->mainHistory[us][from_to(move)] << bonus;
    update_continuation_histories(ss, pos.moved_piece(move), to_sq(move), bonus);

    // Update countermove history
    if (is_ok((ss-1)->currentMove))
    {
        Square prevSq = to_sq((ss-1)->currentMove);
        thisThread->counterMoves[pos.piece_on(prevSq)][prevSq] = move;
    }
  }

  // When playing with strength handicap, choose best move among a set of RootMoves
  // using a statistical rule dependent on 'level'. Idea by Heinz van Saanen.

  Move Skill::pick_best(size_t multiPV) {

    const RootMoves& rootMoves = Threads.main()->rootMoves;
    static PRNG rng(now()); // PRNG sequence should be non-deterministic

    // RootMoves are already sorted by score in descending order
    Value topScore = rootMoves[0].score;
    int delta = std::min(topScore - rootMoves[multiPV - 1].score, PawnValueMg);
    int maxScore = -VALUE_INFINITE;
    double weakness = 120 - 2 * level;

    // Choose best move. For each move score we add two terms, both dependent on
    // weakness. One is deterministic and bigger for weaker levels, and one is
    // random. Then we choose the move with the resulting highest score.
    for (size_t i = 0; i < multiPV; ++i)
    {
        // This is our magic formula
        int push = int((  weakness * int(topScore - rootMoves[i].score)
                        + delta * (rng.rand<unsigned>() % int(weakness))) / 128);

        if (rootMoves[i].score + push >= maxScore)
        {
            maxScore = rootMoves[i].score + push;
            best = rootMoves[i].pv[0];
        }
    }

    return best;
  }

} // namespace


/// MainThread::check_time() is used to print debug info and, more importantly,
/// to detect when we are out of available time and thus stop the search.

void MainThread::check_time() {

  if (--callsCnt > 0)
      return;

  // When using nodes, ensure checking rate is not lower than 0.1% of nodes
  callsCnt = Limits.nodes ? std::min(1024, int(Limits.nodes / 1024)) : 1024;

  static TimePoint lastInfoTime = now();

  TimePoint elapsed = Time.elapsed();
  TimePoint tick = Limits.startTime + elapsed;

  if (tick - lastInfoTime >= 1000)
  {
      lastInfoTime = tick;
      dbg_print();
  }

  // We should not stop pondering until told so by the GUI
  if (ponder)
      return;

  if (   (Limits.use_time_management() && (elapsed > Time.maximum() - 10 || stopOnPonderhit))
      || (Limits.movetime && elapsed >= Limits.movetime)
      || (Limits.nodes && Threads.nodes_searched() >= (uint64_t)Limits.nodes))
      Threads.stop = true;
}


/// UCI::pv() formats PV information according to the UCI protocol. UCI requires
/// that all (if any) unsearched PV lines are sent using a previous search score.

string UCI::pv(const Position& pos, Depth depth) {

  std::stringstream ss;
  TimePoint elapsed = Time.elapsed() + 1;
  const RootMoves& rootMoves = pos.this_thread()->rootMoves;
  size_t pvIdx = pos.this_thread()->pvIdx;
  size_t multiPV = std::min((size_t)Options["MultiPV"], rootMoves.size());
  uint64_t nodesSearched = Threads.nodes_searched();
  uint64_t tbHits = Threads.tb_hits() + (TB::RootInTB ? rootMoves.size() : 0);

  for (size_t i = 0; i < multiPV; ++i)
  {
      bool updated = rootMoves[i].score != -VALUE_INFINITE;

      if (depth == 1 && !updated && i > 0)
          continue;

      Depth d = updated ? depth : std::max(1, depth - 1);
      Value v = updated ? rootMoves[i].uciScore : rootMoves[i].previousScore;

      if (v == -VALUE_INFINITE)
          v = VALUE_ZERO;

      bool tb = TB::RootInTB && abs(v) < VALUE_MATE_IN_MAX_PLY;
      v = tb ? rootMoves[i].tbScore : v;

      if (ss.rdbuf()->in_avail()) // Not at first line
          ss << "\n";

      ss << "info"
         << " depth "    << d
         << " seldepth " << rootMoves[i].selDepth
         << " multipv "  << i + 1
         << " score "    << UCI::value(v);

      if (Options["UCI_ShowWDL"])
          ss << UCI::wdl(v, pos.game_ply());

      if (i == pvIdx && !tb && updated) // tablebase- and previous-scores are exact
         ss << (rootMoves[i].scoreLowerbound ? " lowerbound" : (rootMoves[i].scoreUpperbound ? " upperbound" : ""));

      ss << " nodes "    << nodesSearched
         << " nps "      << nodesSearched * 1000 / elapsed
         << " hashfull " << TT.hashfull()
         << " tbhits "   << tbHits
         << " time "     << elapsed
         << " pv";

      for (Move m : rootMoves[i].pv)
          ss << " " << UCI::move(m, pos.is_chess960());
  }

  return ss.str();
}


/// RootMove::extract_ponder_from_tt() is called in case we have no ponder move
/// before exiting the search, for instance, in case we stop the search during a
/// fail high at root. We try hard to have a ponder move to return to the GUI,
/// otherwise in case of 'ponder on' we have nothing to think on.

bool RootMove::extract_ponder_from_tt(Position& pos) {

    StateInfo st;
    ASSERT_ALIGNED(&st, Eval::NNUE::CacheLineSize);

    bool ttHit;

    assert(pv.size() == 1);

    if (pv[0] == MOVE_NONE)
        return false;

    pos.do_move(pv[0], st);
    TTEntry* tte = TT.probe(pos.key(), ttHit);

    if (ttHit)
    {
        Move m = tte->move(); // Local copy to be SMP safe
        if (MoveList<LEGAL>(pos).contains(m))
            pv.push_back(m);
    }

    pos.undo_move(pv[0]);
    return pv.size() > 1;
}

void Tablebases::rank_root_moves(Position& pos, Search::RootMoves& rootMoves) {

    RootInTB = false;
    UseRule50 = bool(Options["Syzygy50MoveRule"]);
    ProbeDepth = int(Options["SyzygyProbeDepth"]);
    Cardinality = int(Options["SyzygyProbeLimit"]);
    bool dtz_available = true;

    // Tables with fewer pieces than SyzygyProbeLimit are searched with
    // ProbeDepth == DEPTH_ZERO
    if (Cardinality > MaxCardinality)
    {
        Cardinality = MaxCardinality;
        ProbeDepth = 0;
    }

    if (Cardinality >= popcount(pos.pieces()) && !pos.can_castle(ANY_CASTLING))
    {
        // Rank moves using DTZ tables
        RootInTB = root_probe(pos, rootMoves);

        if (!RootInTB)
        {
            // DTZ tables are missing; try to rank moves using WDL tables
            dtz_available = false;
            RootInTB = root_probe_wdl(pos, rootMoves);
        }
    }

    if (RootInTB)
    {
        // Sort moves according to TB rank
        std::stable_sort(rootMoves.begin(), rootMoves.end(),
                  [](const RootMove &a, const RootMove &b) { return a.tbRank > b.tbRank; } );

        // Probe during search only if DTZ is not available and we are winning
        if (dtz_available || rootMoves[0].tbScore <= VALUE_DRAW)
            Cardinality = 0;
    }
    else
    {
        // Clean up if root_probe() and root_probe_wdl() have failed
        for (auto& m : rootMoves)
            m.tbRank = 0;
    }
}

} // namespace Stockfish
