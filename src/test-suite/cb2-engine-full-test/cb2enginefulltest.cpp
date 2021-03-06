#include <QtCore/QString>
#include <QtTest/QtTest>

#include "nlp-engine/cb2engine.h"
#include "nlp-engine/lemmatizerfactory.h"
#include "common/settingskeys.h"
#include "common/settings.h"

#include "ruledef.h"

#define UTF8        QString::fromUtf8

using namespace Lvk;

//--------------------------------------------------------------------------------------------------
// Cb2EnginefullTest
//--------------------------------------------------------------------------------------------------

class Cb2EnginefullTest : public QObject
{
    Q_OBJECT

public:
    Cb2EnginefullTest();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void testCase1();
    void testCase1_data();

private:
    Nlp::Cb2Engine *m_engine;
    int m_ruleSet;

    void testWithRuleSet(int set);
};

//--------------------------------------------------------------------------------------------------

Cb2EnginefullTest::Cb2EnginefullTest()
    : m_engine(0), m_ruleSet(0)
{
    Cmn::Settings().setValue(SETTING_APP_LANGUAGE, "es_AR");

    m_engine = new Nlp::Cb2Engine();
    m_engine->setLemmatizer(Nlp::LemmatizerFactory().createLemmatizer());
}

//--------------------------------------------------------------------------------------------------

void Cb2EnginefullTest::initTestCase()
{
}

//--------------------------------------------------------------------------------------------------

void Cb2EnginefullTest::cleanupTestCase()
{
}

//--------------------------------------------------------------------------------------------------

void Cb2EnginefullTest::testWithRuleSet(int set)
{
    QFETCH(QString, input);
    QFETCH(QString, expOutput);
    QFETCH(int, ruleId);
    QFETCH(int, inputIdx);

    if (m_ruleSet != set) {
        switch (set) {
        case 1:
            setRules1(m_engine);
            break;
        case 2:
            setRules2(m_engine);
            break;
        default:
            m_engine->clear();
            break;
        }
        m_ruleSet = set;
    }

    Nlp::Engine::MatchList matches;
    QString output = m_engine->getResponse(input, matches);

    if (!expOutput.isNull()) {
        QCOMPARE(output, expOutput);
        QVERIFY(matches.size() > 0);
        QCOMPARE(ruleId, static_cast<int>(matches[0].first));
        QCOMPARE(inputIdx, matches[0].second);
    } else {
        QVERIFY(output.isEmpty());
        QVERIFY(matches.isEmpty());
    }
}

//--------------------------------------------------------------------------------------------------

void Cb2EnginefullTest::testCase1()
{
    testWithRuleSet(1);
}

//--------------------------------------------------------------------------------------------------

void Cb2EnginefullTest::testCase1_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<QString>("expOutput");
    QTest::addColumn<int>("ruleId");
    QTest::addColumn<int>("inputIdx");

    QTest::newRow(" 0") << "Hola"                           << RULE_1_OUTPUT_1  << RULE_1_ID  << 0;
    QTest::newRow("0a") << "\"Hola"                         << RULE_1_OUTPUT_1  << RULE_1_ID  << 0; // Test for issue #24
    QTest::newRow("0b") << "{(Hola"                         << RULE_1_OUTPUT_1  << RULE_1_ID  << 0; // Similar to issue #24
    QTest::newRow(" 1") << "HOLAAA!!"                       << RULE_1_OUTPUT_1  << RULE_1_ID  << 0;
    QTest::newRow(" 2") << "hooola$%&/()^[]{}-_.,=:;'\\!?"  << RULE_1_OUTPUT_1  << RULE_1_ID  << 0;
    QTest::newRow(" 3") << "Hola como andas?"               << RULE_1_OUTPUT_1  << RULE_1_ID  << 1;
    QTest::newRow(" 3") << "Hola, que hacessss?"            << RULE_1_OUTPUT_1  << RULE_1_ID  << 1;
    QTest::newRow(" 4") << "chau"                           << RULE_2_OUTPUT_1  << RULE_2_ID  << 0;
    QTest::newRow(" 5") << "CHAUUU CHE..."                  << RULE_2_OUTPUT_1  << RULE_2_ID  << 0;
    QTest::newRow(" 6") << "bueno chau"                     << RULE_2_OUTPUT_1  << RULE_2_ID  << 1;
    QTest::newRow(" 7") << "hey hola"                       << QString()        << 0          << 1;
    QTest::newRow(" 8") << "Bye"                            << QString()        << 0          << 1;
    QTest::newRow(" 9") << "jugar futbol"                   << RULE_3_OUTPUT_1  << RULE_3_ID  << 0;
    QTest::newRow("10") << "yo jugaba futbol"               << RULE_3_OUTPUT_1  << RULE_3_ID  << 0;

    QTest::newRow("11") << UTF8("Yo jugu\xc3\xa9 al futbol mucho tiempo") << RULE_3_OUTPUT_1       << RULE_3_ID  << 0;
    QTest::newRow("12") << UTF8("Yo?? jugu\xc3\xa9 al futbol muuucho tiempo!") << RULE_3_OUTPUT_1  << RULE_3_ID  << 0;

    QTest::newRow("13") << "te gusta el chocolate?"         << "Si me gusta el chocolate"          << RULE_4_ID  << 0;
    QTest::newRow("14") << "te gusta los chocolates?"       << "Si me gusta los chocolates"        << RULE_4_ID  << 0; // Test for issue #5
    QTest::newRow("15") << "te gusta el chocolate mucho?"   << "Si me gusta el chocolate"          << RULE_4_ID  << 1;
    QTest::newRow("16") << "te gusta A mas que B?"          << "Entre A y B no se..."              << RULE_5_ID  << 0;
    QTest::newRow("17") << "Solamente simplemente te gusta A mas que B?"<< "Entre A y B no se..."  << RULE_6_ID  << 1;
    QTest::newRow("18") << "w1 v1a v1b w2 v2 w3 hola w4"    << "w5 v2 w6 Hola! w7 v1a v1b w8"      << RULE_7_ID  << 0;
    QTest::newRow("27") << "le tengo miedo a los aviones"   << "desde cuando le tenes miedo a los aviones?" << RULE_10_ID << 0; // Test for issue #7

    QTest::newRow("19") << ":)"                             << RULE_8_OUTPUT_1  << RULE_8_ID  << 0;
    QTest::newRow("20") << ":D"                             << RULE_8_OUTPUT_1  << RULE_8_ID  << 1;
    QTest::newRow("21") << ":-)"                            << RULE_8_OUTPUT_1  << RULE_8_ID  << 2;
    QTest::newRow("22") << "=)"                             << RULE_8_OUTPUT_1  << RULE_8_ID  << 3;
    QTest::newRow("24") << "perro"                          << QString()        << 0          << 0;
    QTest::newRow("25") << "perros"                         << RULE_9_OUTPUT_1  << RULE_9_ID  << 0;
    QTest::newRow("26") << "perrito"                        << RULE_9_OUTPUT_1  << RULE_9_ID  << 1;
    QTest::newRow("27") << " a b c d e f"                   << RULE_11_OUTPUT_1 << RULE_11_ID << 0; // test for issue #30
    QTest::newRow("28") << " a x b x c d e f x"             << RULE_11_OUTPUT_1 << RULE_11_ID << 0; // idem
    QTest::newRow("28") << " a x b x c x d x e x f"         << RULE_11_OUTPUT_1 << RULE_11_ID << 0; // idem
    QTest::newRow("28") << " x a x b x c x d x e x f"       << QString()        << 0          << 0; // idem
    QTest::newRow("30") << "me gusta jugar a futbol"        << "a mi tambien"   << RULE_13_ID << 0;
    QTest::newRow("31") << "me gusta jugar a VOLEY"         << "a mi no"        << RULE_13_ID << 0;
    QTest::newRow("32") << "me gusta jugar a tenis"         << "Si? tenis?"     << RULE_13_ID << 0;
    QTest::newRow("33") << "w1 v1 v2 w2"                    << "r1"             << RULE_14_ID << 0;
    QTest::newRow("34") << "w1 v2 w2"                       << "r2"             << RULE_14_ID << 0;
    QTest::newRow("34") << "w1 v3 w2"                       << "r3"             << RULE_14_ID << 0;

    QTest::newRow("35") << UTF8("Tengo 18 a\xc3\xb1os")     << "igual que yo!"         << RULE_15_ID << 0;
    QTest::newRow("36") << UTF8("Tengo 20 a\xc3\xb1os")     << "sos mas grande que yo" << RULE_15_ID << 0;
    QTest::newRow("37") << UTF8("Tengo 40 a\xc3\xb1os")     << "sos un viejo"          << RULE_15_ID << 0;
    QTest::newRow("38") << UTF8("Tengo 10 a\xc3\xb1os")     << "sos mas chico que yo"  << RULE_15_ID << 0;
    QTest::newRow("39") << UTF8("Tengo -9 a\xc3\xb1os")     << "imposible!"            << RULE_15_ID << 0;
    QTest::newRow("40") << UTF8("Tengo muuuuchos a\xc3\xb1os") << "sos un viejo"       << RULE_15_ID << 0;

    // Test cases for bug #36
    QTest::newRow("b #36a") << "Soy la diva"                << RULE_23_OUTPUT_1 << RULE_23_ID << 0;
    QTest::newRow("b #36b") << "Soy la diva. de hoy"        << RULE_23_OUTPUT_1 << RULE_23_ID << 0;
    QTest::newRow("b #36c") << "Soy la diva."               << RULE_23_OUTPUT_1 << RULE_23_ID << 0;
    QTest::newRow("b #36d") << "Soy la diva.."              << RULE_23_OUTPUT_1 << RULE_23_ID << 0;
    QTest::newRow("b #36e") << "Soy la diva..."             << RULE_23_OUTPUT_1 << RULE_23_ID << 0;
}

//--------------------------------------------------------------------------------------------------

QTEST_APPLESS_MAIN(Cb2EnginefullTest)

#include "cb2enginefulltest.moc"
