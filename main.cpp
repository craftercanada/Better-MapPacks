#include <Geode/Geode.hpp>
#include <Geode/modify/CreatorLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/binding/LevelBrowserLayer.hpp>
#include <Geode/binding/GJSearchObject.hpp>

using namespace geode::prelude;

struct ProgressionLevel {
    int id;
    const char* difficulty;
    const char* name;
};

struct ProgressionPage {
    const char* title;
    std::vector<ProgressionLevel> levels;
};

static int currentPage = 0;

static std::vector<ProgressionPage> pages = {
    {
        "Dual Progression I",
        {
            {1360404, "Easy Demon", "Demon in the House"},
            {27786218, "Medium Demon", "Mechanical Showdown"},
            {59315849, "Hard Demon", "Double Dash"},
            {35610305, "Insane Demon", "Spacelocked II"},
            {7174110, "Beginner Extreme", "The Ultimate Phase"}
        }
    },
    {
        "Memory Progression I",
        {
            {80433444, "Easy Demon", "Maymory"},
            {77492867, "Medium Demon", "Color Crusher"},
            {4192768, "Hard Demon", "Ditched Machine"},
            {70357068, "Insane Demon", "Poeyeng Aeng"},
            {76196489, "Beginner Extreme", "Troll Level"}
        }
    },
    {
        "Wave Progression I",
        {
            {2997354, "Easy Demon", "Decode"},
            {77614559, "Medium Demon", "Red Dart"},
            {4284013, "Hard Demon", "Nine Circles"},
            {72240540, "Insane Demon", "Super Mario wave"},
            {3979721, "Beginner Extreme", "Cataclysm"}
        }
    }
};

static bool isProgressionLevel(int id) {
    for (auto const& page : pages) {
        for (auto const& level : page.levels) {
            if (level.id == id) return true;
        }
    }
    return false;
}

static int getCompletedCountForPage() {
    int count = 0;

    for (auto const& level : pages[currentPage].levels) {
        if (level.id == 0) continue;

        if (Mod::get()->getSavedValue<bool>(
            fmt::format("completed-{}", level.id),
            false
        )) {
            count++;
        }
    }

    return count;
}

static int getRealLevelCountForPage() {
    int count = 0;

    for (auto const& level : pages[currentPage].levels) {
        if (level.id != 0) count++;
    }

    return count;
}

class ProgressionLayer : public CCLayer {
public:
    static CCScene* scene() {
        auto scene = CCScene::create();
        auto layer = ProgressionLayer::create();
        scene->addChild(layer);
        return scene;
    }

    bool init() override {
        if (!CCLayer::init()) return false;

        auto winSize = CCDirector::get()->getWinSize();

        auto bg = CCSprite::create("GJ_gradientBG.png");
        bg->setAnchorPoint({0.f, 0.f});
        bg->setScaleX(winSize.width / bg->getContentSize().width);
        bg->setScaleY(winSize.height / bg->getContentSize().height);
        bg->setColor({0, 80, 170});
        this->addChild(bg, -10);

        auto title = CCLabelBMFont::create("Progression", "bigFont.fnt");
        title->setPosition({winSize.width / 2, winSize.height - 28.f});
        title->setScale(0.7f);
        this->addChild(title);

        auto subtitle = CCLabelBMFont::create(
            pages[currentPage].title,
            "goldFont.fnt"
        );
        subtitle->setPosition({winSize.width / 2, winSize.height - 58.f});
        subtitle->setScale(0.55f);
        this->addChild(subtitle);

        auto progressText = fmt::format(
            "{} / {}",
            getCompletedCountForPage(),
            getRealLevelCountForPage()
        );

        auto progress = CCLabelBMFont::create(progressText.c_str(), "bigFont.fnt");
        progress->setPosition({winSize.width - 55.f, winSize.height - 30.f});
        progress->setScale(0.45f);
        this->addChild(progress);

        auto menu = CCMenu::create();
        menu->setPosition({0.f, 0.f});
        this->addChild(menu);

        float startY = winSize.height - 105.f;
        float gap = 42.f;

        for (int i = 0; i < pages[currentPage].levels.size(); i++) {
            this->makeRow(menu, pages[currentPage].levels[i], startY - gap * i);
        }

        auto backSpr = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
        auto backBtn = CCMenuItemSpriteExtra::create(
            backSpr,
            this,
            menu_selector(ProgressionLayer::onBack)
        );
        backBtn->setPosition({25.f, winSize.height - 25.f});
        menu->addChild(backBtn);

        auto leftSpr = CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png");
        auto leftBtn = CCMenuItemSpriteExtra::create(
            leftSpr,
            this,
            menu_selector(ProgressionLayer::onLeftPage)
        );
        leftBtn->setPosition({40.f, 25.f});
        menu->addChild(leftBtn);

        auto rightSpr = CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png");
        rightSpr->setFlipX(true);

        auto rightBtn = CCMenuItemSpriteExtra::create(
            rightSpr,
            this,
            menu_selector(ProgressionLayer::onRightPage)
        );
        rightBtn->setPosition({winSize.width - 40.f, 25.f});
        menu->addChild(rightBtn);

        auto pageText = fmt::format("{} / {}", currentPage + 1, pages.size());
        auto pageLabel = CCLabelBMFont::create(pageText.c_str(), "bigFont.fnt");
        pageLabel->setPosition({winSize.width / 2, 25.f});
        pageLabel->setScale(0.4f);
        this->addChild(pageLabel);

        this->setKeypadEnabled(true);
        return true;
    }

    void makeRow(CCMenu* menu, ProgressionLevel const& level, float y) {
        auto winSize = CCDirector::get()->getWinSize();

        auto rowBG = CCScale9Sprite::create("square02_small.png");
        rowBG->setContentSize({380.f, 36.f});
        rowBG->setColor({135, 78, 38});
        rowBG->setOpacity(190);
        rowBG->setPosition({winSize.width / 2, y});
        this->addChild(rowBG);

        bool completed = false;

        if (level.id != 0) {
            completed = Mod::get()->getSavedValue<bool>(
                fmt::format("completed-{}", level.id),
                false
            );
        }

        std::string mainText = fmt::format("{}: {}", level.difficulty, level.name);
        if (completed) mainText += "  DONE";

        auto nameLabel = CCLabelBMFont::create(mainText.c_str(), "bigFont.fnt");
        nameLabel->setAnchorPoint({0.f, 0.5f});
        nameLabel->setPosition({75.f, y + 6.f});
        nameLabel->setScale(0.32f);
        this->addChild(nameLabel);

        std::string idText = level.id == 0
            ? "Level not chosen yet"
            : fmt::format("ID: {}", level.id);

        auto idLabel = CCLabelBMFont::create(idText.c_str(), "chatFont.fnt");
        idLabel->setAnchorPoint({0.f, 0.5f});
        idLabel->setPosition({75.f, y - 10.f});
        idLabel->setScale(0.55f);
        this->addChild(idLabel);

        auto buttonSprite = ButtonSprite::create(level.id == 0 ? "TBD" : "Play");
        buttonSprite->setScale(0.65f);

        auto button = CCMenuItemSpriteExtra::create(
            buttonSprite,
            this,
            menu_selector(ProgressionLayer::onOpenLevel)
        );

        button->setUserData(reinterpret_cast<void*>(static_cast<intptr_t>(level.id)));
        button->setPosition({winSize.width - 70.f, y});
        menu->addChild(button);
    }

    void onOpenLevel(CCObject* sender) {
        auto node = static_cast<CCNode*>(sender);
        int levelID = static_cast<int>(reinterpret_cast<intptr_t>(node->getUserData()));

        if (levelID == 0) {
            FLAlertLayer::create(
                "TBD",
                "This level has not been picked yet.",
                "OK"
            )->show();
            return;
        }

        auto search = GJSearchObject::create(SearchType::Search, std::to_string(levelID));
        auto scene = LevelBrowserLayer::scene(search);

        CCDirector::get()->pushScene(
            CCTransitionFade::create(0.5f, scene)
        );
    }

    void reloadScene() {
        CCDirector::get()->replaceScene(
            CCTransitionFade::create(0.25f, ProgressionLayer::scene())
        );
    }

    void onLeftPage(CCObject*) {
        currentPage--;

        if (currentPage < 0) {
            currentPage = pages.size() - 1;
        }

        this->reloadScene();
    }

    void onRightPage(CCObject*) {
        currentPage++;

        if (currentPage >= pages.size()) {
            currentPage = 0;
        }

        this->reloadScene();
    }

    void onBack(CCObject*) {
        CCDirector::get()->popSceneWithTransition(
            0.5f,
            PopTransition::kPopTransitionFade
        );
    }

    void keyBackClicked() override {
        this->onBack(nullptr);
    }

    CREATE_FUNC(ProgressionLayer);
};

class $modify(ProgressionCreatorLayer, CreatorLayer) {
    void onMapPacks(CCObject* sender) {
        bool useOriginal = Mod::get()->getSettingValue<bool>("use-original-map-packs");

        if (useOriginal) {
            CreatorLayer::onMapPacks(sender);
            return;
        }

        CCDirector::get()->pushScene(
            CCTransitionFade::create(0.5f, ProgressionLayer::scene())
        );
    }
};

class $modify(ProgressionPlayLayer, PlayLayer) {
    void levelComplete() {
        PlayLayer::levelComplete();

        if (!this->m_level) return;

        int levelID = this->m_level->m_levelID.value();

        if (isProgressionLevel(levelID)) {
            Mod::get()->setSavedValue<bool>(
                fmt::format("completed-{}", levelID),
                true
            );
        }
    }
};