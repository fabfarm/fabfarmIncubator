import { Slider } from "@material-tailwind/react";
import React from "preact/compat";

export default function SliderControl({
  min,
  max,
  label,
  value,
  setValue,
  unit,
}: {
  label?: string;
  min?: number;
  max?: number;
  value: number;
  setValue: (value: number) => void;
  unit?: string;
}) {
  return (
    <div className="flex flex-col gap-2">
      <h5 className="flex justify-center font-semibold">
        {label}
        {label && ": "}
        {value.toFixed(1)}
        {unit}
      </h5>
      <Slider
        max={max}
        min={min}
        step="0.1"
        value={value}
        onChange={(event: React.ChangeEvent<HTMLInputElement>) =>
          setValue(Number(event.currentTarget.value))
        }
        size="sm"
      />
    </div>
  );
}
