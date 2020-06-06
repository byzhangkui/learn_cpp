#include <memory>
#include <iostream>
#include <vector>

struct B {
  virtual void foo() { std::cout << "B::foo\n"; }
  virtual ~B() = default;
};

struct D : public B {
  D() { std::cout << "D::D()\n"; }
  ~D() { std::cout << "D::~D()\n"; }
  void foo() override {std::cout << "D::foo\n"; }
};

struct Foo {
  Foo(int val) { val_ = val; }
  ~Foo() { std::cout << "~Foo\n"; }
  int val_;
};

int main() {
  {
    std::unique_ptr<D> p(new D); //p is a unique_ptr that owns a D
    auto d = std::make_unique<D>();// since c++14
    auto c = std::move(d);
    assert(!d);//now d owns nothing and holds a null point
    c->foo();//c owns the D object
    c.reset(new D());//replace D with new one, calls deleter for old one
    c.reset(nullptr);//release and delete the owned D
  }

  std::cout << "swap\n";
  {
    std::unique_ptr<Foo> f1 = std::make_unique<Foo>(1);
    std::unique_ptr<Foo> f2 = std::make_unique<Foo>(2);
    f1.swap(f2);
    std::cout << "f1->val_:" << f1->val_ << std::endl;
    std::cout << "f2->val_:" << f2->val_ << std::endl;
  }

  std::cout << "release\n";
  {
    std::unique_ptr<Foo> f1 = std::make_unique<Foo>(1);
    Foo* foo = f1.release();//Releases the ownership of the managed object if any.The caller is responsible for deleting the object.
    std::cout << "foo->val_:" << foo->val_ << std::endl;
    delete foo;
  }

  std::cout << "runtime polymorphism \n";
  { 
    std::unique_ptr<B> b = std::make_unique<D>(); //b owns D as a point to base
    std::vector<std::unique_ptr<B>> v;
    v.push_back(std::make_unique<D>());
    v.push_back(std::move(b));
    v.emplace_back(new D());
    for (auto& item : v) {
      item->foo();
    }
    //D::~D called 3 times
  }

  std::cout << "Custom lambda-expression deleter demo\n";
  {
    std::unique_ptr<D, std::function<void(D*)>> p(new D, [](D* ptr) {
      std::cout << "destroying from a custom deleter...\n";
      delete ptr;
    });
    p->foo();
  }

  return 0;
}