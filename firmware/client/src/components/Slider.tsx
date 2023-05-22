import { Slider } from "@material-tailwind/react";
import debounce from "just-debounce-it";
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
  const debouncer = debounce(
    (event: React.ChangeEvent<HTMLDivElement>) =>
      setValue(Number((event.target as Record<string, any>).value)),
    1000
  );

  return (
    <div className="flex flex-col gap-2">
      <h5 className="flex justify-center font-semibold">
        {label}
        {label && ": "}
        {value ?? "N/A"}
        {unit}
      </h5>
      <Slider
        max={max}
        min={min}
        step="0.1"
        defaultValue={value ?? "N/A"}
        onChange={debouncer}
        size="sm"
      />
    </div>
  );
}
