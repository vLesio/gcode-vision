#ifndef MATRIX_STACK_H
#define MATRIX_STACK_H

#include <stack>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class MatrixStack {
private:
    std::stack<glm::mat4> stack;

public:
    MatrixStack() {
        stack.push(glm::mat4(1.0f));
    }

    glm::mat4& top() {
        return stack.top();
    }

    const glm::mat4& top() const {
        return stack.top();
    }

    void push() {
        stack.push(stack.top());
    }

    void pop() {
        if (stack.size() > 1)
            stack.pop();
    }

    void loadIdentity() {
        stack.top() = glm::mat4(1.0f);
    }

    void translate(const glm::vec3& v) {
        stack.top() = glm::translate(stack.top(), v);
    }

    void rotate(float angleRad, const glm::vec3& axis) {
        stack.top() = glm::rotate(stack.top(), angleRad, axis);
    }

    void scale(const glm::vec3& s) {
        stack.top() = glm::scale(stack.top(), s);
    }

    void setTop(const glm::mat4& mat) {
        if (!stack.empty()) {
            stack.top() = mat;
        }
    }
};

#endif // MATRIX_STACK_H
