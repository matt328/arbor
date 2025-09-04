#pragma once

namespace gld {

class QtVulkanWidget : public QWidget {
  Q_OBJECT
public:
  QtVulkanWidget(QWidget* parent = nullptr);
  ~QtVulkanWidget() = default;

  QtVulkanWidget(const QtVulkanWidget&) = delete;
  QtVulkanWidget(QtVulkanWidget&&) = delete;
  auto operator=(const QtVulkanWidget&) -> QtVulkanWidget& = delete;
  auto operator=(QtVulkanWidget&&) -> QtVulkanWidget& = delete;

  [[nodiscard]] auto getHandle() const -> WId;

signals:
  void resized(int w, int h, qreal dpr);

protected:
  void resizeEvent(QResizeEvent* event) override;

private:
};

}
