#pragma once
class AnimatioControler
{
protected:
	std::unordered_map<std::string, AnimationClip> clips;
	std::queue<std::string> queue;

	float speed = 1.f;
	bool isPlaying = false;

	AnimationClip* currentClip;
	int currentFrame = -1;
	int totalFrame = 0;
	float clipDuration = 0.f;
	float accumTime = 0.f;

	sf::Sprite* target = nullptr;

public:
	void AddClip(const AnimationClip& newClip);
	void SetTarget(sf::Sprite* sprite) { target = sprite; }
	sf::Sprite* GetTarget() const { return target; }
	bool IsPlaying() { return isPlaying; }
	void SetSpeed(float s) { speed = s; }
	float Speed() const { return speed; }
	std::string GetCurrentClipId();

	void Update(float dt);

	void Play(const std::string& clipid, bool clearQueue = true);
	void PlayQueue(const std::string& clipid);
	void Stop();

	void SetFrame(const AnimationFrame& frame);

	int GetCurrFrame() { return this->currentFrame; }
	int GetTotalFrame() { return this->totalFrame - 1; }
	bool GetLastFrame();
};

