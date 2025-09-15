#pragma once

namespace arb {

struct Semaphore;

struct SemaphorePack {
  Semaphore& graphics;
  Semaphore& transfer;
  Semaphore& compute;
};
}
