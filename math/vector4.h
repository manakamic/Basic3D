#pragma once

namespace math {
    class matrix44;

    class vector4 {
    public:
        // コンストラクタ
        vector4();
        vector4(const double x, const double y, const double z) : x(x), y(y), z(z), w(1.0) {}
        vector4(const double x, const double y, const double z, double w) : x(x), y(y), z(z), w(w) {}
        vector4(const vector4&) = default; // コピー
        vector4(vector4&&) = default; // ムーブ

        // デストラクタ
        virtual ~vector4() = default;

        vector4& operator =(const vector4&) = default; // コピー
        vector4& operator =(vector4&&) = default; // ムーブ

        vector4 operator +(const vector4& rhs) const;
        vector4 operator -(const vector4& rhs) const;

        vector4 operator *(const double rhs) const;
        vector4 operator /(const double rhs) const;

        vector4 operator *(const matrix44 rhs) const;

        void set(const vector4& vector);
        void set(const double x, const double y, const double z);

        void add(const vector4& vector);
        void add(const double x, const double y, const double z);

        double get_x() const { return x; }
        double get_y() const { return y; }
        double get_z() const { return z; }
        double get_w() const { return w; }

        double length() const;

        void normalized();
        vector4 normalize() const;

        double dot(const vector4& rhs) const;
        vector4 cross(const vector4& rhs) const;

    private:
        double x;
        double y;
        double z;
        double w;
    };

} // math
